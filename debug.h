#pragma once

#define DEBUG

#ifdef DEBUG
#define Debug(CodeSegment) {CodeSegment;}
#else
#define Debug(CodeSegment) ((void)0)
#endif

#define Halt(CodeSegment)	while(true){CodeSegment;}