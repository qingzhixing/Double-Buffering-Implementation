#pragma once

#define DEBUG

#ifdef DEBUG
#define Debug(CodeSegment) {CodeSegment;}
#define Judge(CONDITION) \
   if(CONDITION){}        \
   else{ printf("Condition false: %s\n",(#CONDITION)); }
#define ASSERT(CONDITION) \
   if(CONDITION){}        \
   else{ \
		printf("Condition false: %s\n",(#CONDITION)); \
		Halt();\
	}
#else
#define Debug(CodeSegment) ((void)0)
#define Judge(CONDITION) ((void)0)
#define ASSERT(CONDITION) ((void)0)
#endif

#define Halt(CodeSegment)	while(true){CodeSegment;}