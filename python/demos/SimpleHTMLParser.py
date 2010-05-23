# SimpleHTMLParser
# This is a very simple parser intended to extract the body text, title, and
# a list of URLs. It is intended to be as lenient as possible, regarding
# HTML standards and poorly formed HTML.
#
# See http://www.winwaed.com/info/python_html/parser.shtml for more details.
#
# Copyright and License Notice
# ----------------------------
# Copyright (C) 2006 by Winwaed Software Technology.  All rights reserved.  
# Some individual files may be covered by other copyrights.
#
# This material was originally written and compiled by Richard Marsden 
# of Winwaed Software Technology 2004-6.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that this entire copyright notice
# is duplicated in all such copies.
# 
# This software is provided "as is" and without any expressed or implied
# warranties, including, without limitation, the implied warranties of
# merchantibility and fitness for any particular purpose.
#
# Winwaed Software Technology
# http://www.winwaed.com

from string import join, split
from urlparse import urljoin

import htmlentitydefs
import re



# ############################
# HTML Parsing Objects, etc.

def simplify_text(sbuff):
    str = ""
    already_sp = 1
    for c in sbuff:
        if c.isspace():
            if not already_sp:
                already_sp = 1
                str = str + " "
        else:
            str = str + c
            already_sp = 0
    return str.strip();

###################
# This is a basic parsing class. It should be sub-classed and have new
# methods to over-ride the handler methods. See SimpleText HTMLParser
# for this.

class BaseSimpleHTMLParser:
    def __init__(self):
        self.intext="";
    def handle_textData(self, text):
        pass
    def handle_endDocument(self):
        pass
    def handle_startDocument(self):
        pass
    def handle_startTag(self, tag, attrs):
        pass
    def handle_startEndTag(self, tag, attrs):
        pass
    def handle_endTag(self, tag):
        pass
    def handle_entityTag(self, tag):
        # Note: this might be called with "entity candidates" - ie. poorly written
        # entities, or "&" characters that are not properly escaped
        # The inheriting object should try to interpret the entity. If this fails,
        # it should be interpreted as text data
        pass

    def parse(self,sInput):
        self.intext=sInput;
        self.iptr = 0;
        sData = ""
        self.handle_startDocument();
        while (self.iptr<len(self.intext) ):
            ch = self.intext[self.iptr];
            if (ch=='<'):
                # output and empty the data buffer
                if (len(sData)>0):
                    self.handle_textData(sData)
                    sData = ""
                # get the new tag and interpret
                tag = self.fetchTagToClosedAngle();
                if tag[0] == '?':
                    # DOCTYPE or similar - do nothing
                    pass
                elif tag[0]=='!':
                    if (tag[1]=='-' and tag[2]=='-'):
                        # comment - do nothing
                        pass
                    else:
                        # DOCTYPE or similar - do nothing 
                        pass
                elif (tag[0]=='/'):
                    # closed tag
                    endTag = tag[1:].strip().lower();
                    self.handle_endTag(endTag);
                else:
                    # tag
                    self.parseTag(tag)
            elif (ch=='&'):
                # output and empty the data buffer
                if (len(sData)>0):
                    self.handle_textData(sData)
                    sData = ""
                # Fetch the Entity Tag
                tag = self.fetchEntityTag();
                if (len(tag)>0):
                    self.handle_entityTag(tag)
            else:
                sData = sData + ch;
            self.iptr=self.iptr+1;
        # End of document
        self.handle_endDocument();        

    def fetchTagToClosedAngle(self):
        ch = '/'
        st = ""
        while (ch!='>' and self.iptr<len(self.intext) ):
            self.iptr = self.iptr + 1
            ch = self.intext[self.iptr]
            if (ch!='>'):
                st = st + ch
        return st;

    def fetchEntityTag(self):
        ch = 'A'   # dummy char
        st = ""
        while (ch.isalnum() and self.iptr<len(self.intext) ):
            self.iptr = self.iptr + 1
            ch = self.intext[self.iptr]
            if (ch.isalnum()):
                st = st + ch
        return st;
    
    def skipToEndScriptTag(self):
        kptr=0
        escript = "</script>"
        while ( kptr<9 and self.iptr<len(self.intext)-1 ):
            self.iptr = self.iptr + 1
            if self.intext[self.iptr].lower() == escript[kptr]:
                kptr = kptr + 1
            else:
                kptr = 0

    def parseTag(self,tag):
        sbuff = tag.strip()
        bStartEnd=0
        attribs = {}
        if (sbuff[len(sbuff)-1]=='/'):
            bStartEnd=1
            sbuff = sbuff[:len(sbuff)]

        tmatch = re.compile(r'\w+')
        tm = tmatch.search(sbuff)
        ipp = 0
        if (tm):
        	output_tag = tm.group().lower()
        	sbuff = sbuff[ tm.end() : ]
        	attr_match = re.compile(r'(\w+)\s*=\s*"([^"]*)"')
        	m = attr_match.findall( sbuff)
        	if (m):
        		# extracted attributes=>create dictionary
        		for aa in m:
        			attribs[ aa[0] ] = aa[1]
        else:
        	output_tag = sbuff.lower()
        if (bStartEnd):
            self.handle_startEndTag(output_tag, attribs )            
        else:
            if (output_tag=="script"):
                # Skip the script tag and its contents
                self.skipToEndScriptTag()
            else:
                self.handle_startTag(output_tag, attribs )            


####################
# This is the main parser, which implements some of the handlers

class SimpleTextHTMLParser(BaseSimpleHTMLParser):
    def __init__(self):
        self.mytext="";
        self.bodyText="";
        self.titleText="";
        self.bInTitle = 0;
        self.listURLs = [];
        self.fullUniqueURLs = {}
        self.sFullURL=""
    def parse(self,sInput,sURL):
        self.sFullURL = sURL
        BaseSimpleHTMLParser.parse(self,sInput)
    def handle_textData(self, text):
        self.mytext = self.mytext + text;
    def handle_entityTag(self, tag):
        # Note: Unicode is passed through as an entity code
        try:
            if (tag[0]=='#'):
                # entity ASCII code
                chval = 0
                if (tag[1]=='x'):
                    chval = int(tag[2:],16)
                else:
                    chval = int(tag[1:],10)
                if chval>255:
                    self.mytext = self.mytext + "&" + tag + ";"
                else:
                    self.mytext = self.mytext + chr(chval)
            else:   # entity symbol            
                self.mytext = self.mytext + htmlentitydefs.entitydefs[tag];
        except:
            # The above will fail if the entity is actually a poorly escaped '&'
            # If this is the case, record it as text data
            self.mytext = self.mytext + "&" + tag
            if (self.iptr<len(self.intext)):
                self.mytext = self.mytext + self.intext[self.iptr]
            
    def handle_startDocument(self):
        self.mytext="";
    def handle_endDocument(self):
        self.bodyText = simplify_text(self.mytext)
        self.mytext=""
        self.listURLs = self.fullUniqueURLs.keys()
    def handle_startTag(self, tag, attrs):
        if tag=="title":
            self.bInTitle = 1
            self.mytext=""
        elif tag=="a":
            if attrs.has_key("href"):
                sub_url = attrs["href"]
                url = split( urljoin(self.sFullURL,sub_url), '#')[0] 
                if (url[len(url)-1]=='/'):
                    url = url[ : len(url)-1]
                if url[:5] == "http:":
                    self.fullUniqueURLs[url] = 1
        elif tag=="body":
            if (self.bInTitle):
                self.bInTitle = 0
                self.titleText=simplify_text(self.mytext);
            self.myText = ""
    def handle_startEndTag(self, tag, attrs):
        pass    
    def handle_endTag(self, tag):
        if (tag=="title"):
            self.titleText = simplify_text(self.mytext);
            self.myText = ""
            self.bInTitle = 0
        elif (tag=="head"):
            if (self.bInTitle):
                self.bInTitle = 0
                self.titleText=simplify_text(self.mytext);
            self.myText = ""

