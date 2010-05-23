/*
 * Mycelia immersive 3d network visualization tool.
 * Copyright (C) 2008-2009 Sean Whalen.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cuda.h>
#include <cutil.h>
#include <cutil_math.h>
#include <math_functions.h>
#include <stdio.h>

#define MAX_ITERATIONS 300
#define MAX_DELTA 100
#define COOLING_EXPONENT 1.5
#define VOLUME 1000
#define REPULSION_RADIUS 10000

inline __host__ __device__ void operator+=(float3& p, float4 q)
{
    p.x += q.x;
    p.y += q.y;
    p.z += q.z;
}

inline __host__ __device__ void operator+=(float4& p, float3 q)
{
    p.x += q.x;
    p.y += q.y;
    p.z += q.z;
}

__global__ void
updatePositions(int size, float4* positions_d, float4* positions_delta_from, float4* positions_delta_to, float t)
{
    // sum changes
    int row = threadIdx.x;
    float3 delta;
    
    for(int col = 0; col < size; col++)
    {
        // forces on i computed from i, j
        delta += positions_delta_from[row*size + col];
        
        // forces on i computed from j, i
        delta += positions_delta_to[col*size + row];
    }
    
    // scale if change is too large
    float mag = length(delta);
    
    if(mag > t)
    {
        mag = t / mag;
        delta *= mag;
    }
    
    // update position
    positions_d[row] += delta;
}

__global__ void
calculateForces(int size, float4* positions_d, float4* positions_delta_from, float4* positions_delta_to, int* adjacencies_d, float t, float k)
{
    int i = blockIdx.x*blockDim.x + threadIdx.x;
    int j = blockIdx.y*blockDim.y + threadIdx.y;
    if(i == j || i >= size || j >= size) return;
    
    // repel
    float3 v = make_float3(positions_d[i]) - make_float3(positions_d[j]);
    float mag = length(v);
    v /= mag;
    float f = k*k * (1/mag - mag*mag/REPULSION_RADIUS) * positions_d[i].w;
    
    // update relative change
    int offset = i*size + j;
    positions_delta_from[offset] += v * f;
    positions_delta_to[offset] += v * -f;
    
    // attract if connected
    f = (mag*mag/k) * adjacencies_d[offset]; // avoid conditional by setting force to 0 if not adjacent
    
    // update relative change
    positions_delta_from[offset] += v * -f;
    positions_delta_to[offset] += v * f;
}

extern "C"
{
    __host__ void
    gpuLayout(float4* positions_h, int* adjacencies_h, int size)
    {
        /*int device;
        struct cudaDeviceProp prop;
        CUDA_SAFE_CALL(cudaGetDevice(&device));
        CUDA_SAFE_CALL(cudaGetDeviceProperties(&prop, device));
        printf("%d %s %d.%d\n", device, prop.name, prop.major, prop.minor);*/
        
        float k = pow(VOLUME/(float)size, 1/3.0f);
        dim3 dimBlock(20, 20);
        dim3 dimGrid((size+dimBlock.x-1) / dimBlock.x, (size+dimBlock.y-1) / dimBlock.y);
        
        float4* positions_d;
        CUDA_SAFE_CALL(cudaMalloc((void**)&positions_d, sizeof(float4)*size));
        cudaMemcpy(positions_d, positions_h, sizeof(float4)*size, cudaMemcpyHostToDevice);
        
        float4* positions_delta_from;
        CUDA_SAFE_CALL(cudaMalloc((void**)&positions_delta_from, sizeof(float4)*size*size));
        
        float4* positions_delta_to;
        CUDA_SAFE_CALL(cudaMalloc((void**)&positions_delta_to, sizeof(float4)*size*size));
        
        int* adjacencies_d;
        CUDA_SAFE_CALL(cudaMalloc((void**)&adjacencies_d, sizeof(int)*size*size));
        cudaMemcpy(adjacencies_d, adjacencies_h, sizeof(int)*size*size, cudaMemcpyHostToDevice);
        
        for(int i = MAX_ITERATIONS; i >= 0; i--)
        {
            float t = MAX_DELTA * pow(i/(double)MAX_ITERATIONS, COOLING_EXPONENT);
            
            cudaMemset(positions_delta_from, 0, sizeof(float4)*size*size);
            cudaMemset(positions_delta_to, 0, sizeof(float4)*size*size);
            
            calculateForces<<<dimGrid, dimBlock>>>(size, positions_d, positions_delta_from, positions_delta_to, adjacencies_d, t, k);
            updatePositions<<<1, size>>>(size, positions_d, positions_delta_from, positions_delta_to, t);
            
            cudaThreadSynchronize();
        }
        
        cudaMemcpy(positions_h, positions_d, sizeof(float4)*size, cudaMemcpyDeviceToHost);
        cudaFree(positions_d);
        cudaFree(positions_delta_from);
        cudaFree(positions_delta_to);
        cudaFree(adjacencies_d);
    }
}