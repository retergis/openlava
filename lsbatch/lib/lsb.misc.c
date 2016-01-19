/*
 * Copyright (C) 2014-2015 David Bigagli
 * Copyright (C) 2007 Platform Computing Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 *
 */

#include "lsb.h"

void
initTab(struct hTab *tabPtr)
{
    if (tabPtr) {
        h_initTab_ (tabPtr, 50);
    }
}

hEnt *
addMemb(struct hTab *tabPtr, LS_LONG_INT member)
{
    char memberStr[256];
    hEnt *ent;
    int new;

    if (!tabPtr)
        return NULL;

    sprintf(memberStr, LS_LONG_FORMAT, member);
    ent = h_addEnt_(tabPtr, memberStr, &new);
    if (!new) {
        return NULL;
    }

    return ent;
}

char
remvMemb(struct hTab *tabPtr, LS_LONG_INT member)
{
    char memberStr[256];
    hEnt *ent;

    if (tabPtr) {
        sprintf(memberStr,LS_LONG_FORMAT , member);
        if ((ent = h_getEnt_(tabPtr, memberStr)) == NULL)
	    return false;
        else {
            ent->hData = NULL;
	    h_delEnt_ (tabPtr, ent);
	    return true;
        }
    }
    return false;
}

hEnt *
chekMemb(struct hTab *tabPtr, LS_LONG_INT member)
{
    char memberStr[256];
    hEnt *ent;

    if (tabPtr) {
        sprintf(memberStr, LS_LONG_FORMAT , member);
        ent = h_getEnt_(tabPtr, memberStr);
    }

    return ent;
}

hEnt *
addMembStr(struct hTab *tabPtr, char *memberStr)
{
    hEnt *ent;
    int new;

    if (tabPtr && memberStr) {
        ent = h_addEnt_(tabPtr, memberStr, &new);
        if (!new) {
            return NULL;
        }
        else
	    return(ent);
    }
    return NULL;
}

char
remvMembStr(struct hTab *tabPtr, char *memberStr)
{
    hEnt *ent;

    if (tabPtr && memberStr) {
        if ((ent = h_getEnt_(tabPtr, memberStr)) == NULL)
	    return false;
        else {
            ent->hData = NULL;
	    h_delEnt_(tabPtr, ent);
	    return true;
        }
    }
    return false;
}

hEnt *
chekMembStr(struct hTab *tabPtr, char *memberStr)
{
    hEnt *ent = NULL;

    if (tabPtr && memberStr) {
        ent = h_getEnt_ (tabPtr, memberStr);
    }

    return ent;
}

struct sortIntList *
initSortIntList(int increased)
{
    struct sortIntList *headerPtr;

    headerPtr = malloc(sizeof(struct sortIntList));
    headerPtr->forw = headerPtr->back = headerPtr;
    headerPtr->value = increased;
    return headerPtr;
}

int
insertSortIntList(struct sortIntList *header, int value)
{
    struct sortIntList *listPtr;
    struct sortIntList *newPtr;

    listPtr = header->forw;
    while (listPtr != header) {

	if (listPtr->value == value)
	    return 0;
        if (header->value) {

	    if (listPtr->value > value)
	        break;
	    listPtr=listPtr->forw;
	} else {

	    if (listPtr->value < value)
	        break;
	    listPtr=listPtr->forw;
	}
    }
    newPtr = malloc(sizeof (struct sortIntList));
    newPtr->forw = listPtr;
    newPtr->back = listPtr->back;
    listPtr->back->forw=newPtr;
    listPtr->back=newPtr;
    newPtr->value = value;

    return 0;
}

struct sortIntList *
getNextSortIntList(struct sortIntList *header,
    struct sortIntList *current, int *value)
{
    struct sortIntList *nextPtr;

    nextPtr = current->forw;
    if (nextPtr == header)
	return NULL;
    *value = nextPtr->value;
    return(nextPtr);

}

void
freeSortIntList(struct sortIntList *header)
{
    struct sortIntList *listPtr;
    struct sortIntList *nextPtr;

    listPtr = header->forw;
    while (listPtr != header) {
	nextPtr = listPtr->forw;
	free(listPtr);
	listPtr = nextPtr;
    }
    free(listPtr);
    return;

}

int
getMinSortIntList(struct sortIntList *header, int *minValue)
{
    if (header == header->forw)
	return -1;
    if (header->value)
	*minValue = header->forw->value;
    else
	*minValue = header->back->value;
    return 0;

}

int
getMaxSortIntList(struct sortIntList *header, int *maxValue)
{

    if (header == header->forw)
	return -1;
    if (header->value)
	*maxValue = header->back->value;
    else
	*maxValue = header->forw->value;
    return 0;

}

int
getTotalSortIntList(struct sortIntList *header)
{
    struct sortIntList *listPtr;
    int total=0;

    listPtr=header->forw;
    while(listPtr != header) {
	total++;
	listPtr = listPtr->forw;
    }
    return(total);

}

int
sndJobFile_(int s, struct lenData *jf)
{
    int nlen = htonl(jf->len);

    if (b_write_fix(s, NET_INTADDR_(&nlen), NET_INTSIZE_) != NET_INTSIZE_) {
	lsberrno = LSBE_SYS_CALL;
	return -1;
    }

    if (b_write_fix(s, jf->data, jf->len) != jf->len) {
	lsberrno = LSBE_SYS_CALL;
	return -1;
    }

    return 0;
}


void
upperStr(char *in, char *out)
{
    for (; *in != '\0'; in++, out++)
	*out = toupper(*in);
    *out = '\0';
}


void
copyJUsage(struct jRusage *to, struct jRusage *from)
{
    struct pidInfo *newPidInfo;
    int *newPgid;

    to->mem = from->mem;
    to->swap = from->swap;
    to->utime = from->utime;
    to->stime = from->stime;

    if (from->npids) {
        newPidInfo =  calloc(from->npids, sizeof(struct pidInfo));
        if (newPidInfo != NULL) {
            if (to->npids)
                FREEUP (to->pidInfo);
            to->pidInfo = newPidInfo;
            to->npids = from->npids;
            memcpy(to->pidInfo,
                   from->pidInfo,
                   from->npids * sizeof(struct pidInfo));
        }
    } else if (to->npids) {
        FREEUP (to->pidInfo);
        to->npids = 0;
    }

    if (from->npgids) {
        newPgid = calloc(from->npgids, sizeof(int));
        if (newPgid == NULL) return;
        if (to->npgids)
            FREEUP (to->pgid);
        to->pgid = newPgid;
        to->npgids = from->npgids;
        memcpy(to->pgid,
               from->pgid,
               from->npgids * sizeof(int));
    } else if (to->npgids) {
        FREEUP (to->pgid);
        to->npgids = 0;
    }
}

void
convertRLimit(int *pRLimits, int toKb)
{
    int i;

    for (i = 0; i < LSF_RLIM_NLIMITS; i++) {
	switch (i) {
	    case LSF_RLIMIT_FSIZE:
	    case LSF_RLIMIT_DATA:
	    case LSF_RLIMIT_STACK:
	    case LSF_RLIMIT_CORE:
	    case LSF_RLIMIT_RSS:
	    case LSF_RLIMIT_VMEM:
		if (pRLimits[i] > 0) {
		    if (toKb) {
			pRLimits[i] /= 1024;
		    } else {
			pRLimits[i] *= 1024;
		    }
		}
		break;
	}
    }
}

int
limitIsOk_(int *rLimits)
{
#define EXCEED_MAX_INT(x) ( (x) > 0 ? (unsigned int)(x) >> 21 : 0 )

    if ( EXCEED_MAX_INT(rLimits[LSF_RLIMIT_FSIZE])
	 || EXCEED_MAX_INT(rLimits[LSF_RLIMIT_DATA])
	 || EXCEED_MAX_INT(rLimits[LSF_RLIMIT_STACK])
	 || EXCEED_MAX_INT(rLimits[LSF_RLIMIT_CORE])
	 || EXCEED_MAX_INT(rLimits[LSF_RLIMIT_RSS])
	 || EXCEED_MAX_INT(rLimits[LSF_RLIMIT_SWAP]) ) {
	return 0;
    } else {
	return 1;
    }
}

char *
lsb_splitName(char *str, unsigned int *number)
{
    static char fname[]="lsb_splitName";
    static char name[4*MAXLINELEN];
    static int  nameNum;
    int twoPartFlag;
    int i, j;


    if (str == NULL || number == NULL) {
        ls_syslog(LOG_ERR, I18N(5650,"%s: bad input.\n"), fname); /* catgets 5650 */
        return NULL;
    }


    twoPartFlag = 0;
    j = 0;


    for (i=0; i<strlen(str); i++) {
        if (str[i] != '*') {

            name[j] = str[i];
            j++;
        }
        else {

            twoPartFlag = 1;
            name[j] = '\0';
            j = 0;


            sscanf(name, "%d", &nameNum);


            if (nameNum <= 0) {
                nameNum = 1;
                ls_syslog(LOG_ERR, I18N(5651, "%s: bad input format.  Assuming 1 host.\n"), /* catgets 5651 */
			  fname);
            }
        }
    }


    name[j] = '\0';


    if (twoPartFlag == 0 || j == 0) {


        nameNum = 1;
    }

    *number = nameNum;
    return name;
}

struct nameList *
lsb_compressStrList(char **strList, int numStr)
{
    static char   fname[]="lsb_compressStrList";
    static struct nameList nameList;
    int    i;
    int    hIndex=0;
    int    numSameStr;
    int    headPtr;

    if (nameList.names != NULL) {
         for(i = 0; i < nameList.listSize; i++)
             FREEUP(nameList.names[i]);
         FREEUP(nameList.names);
         FREEUP(nameList.counter);
    }

    nameList.listSize = 0;
    nameList.names = NULL;
    nameList.counter = NULL;

    if (numStr <= 0) {
        return &nameList;
    }

    if (strList == NULL)
      return NULL;

    nameList.names = calloc(numStr, sizeof(char *));
    nameList.counter = calloc(numStr,  sizeof(int));

    if (!nameList.names || !nameList.counter)  {
	ls_syslog(LOG_ERR, I18N_FUNC_FAIL_M, fname, "calloc");
        FREEUP(nameList.names);
        FREEUP(nameList.counter);
        return NULL;
    }

    headPtr = 0;
    numSameStr = 1;
    for (i = 1; i < numStr; i++)
        if (strList[i]) {

            if (strcmp(strList[i], strList[headPtr])== 0) {

                numSameStr++;
                 continue;
             }
             nameList.names[hIndex] = putstr_(strList[headPtr]);
             headPtr = i;
             nameList.counter[hIndex] = numSameStr;
             hIndex++;
             numSameStr = 1;
        }

    nameList.names[hIndex] = putstr_(strList[headPtr]);
    nameList.counter[hIndex] = numSameStr;
    hIndex++;
    nameList.listSize = hIndex;

    nameList.names = realloc(nameList.names,
                             nameList.listSize*sizeof(char *));
    nameList.counter = realloc(nameList.counter,
                               nameList.listSize*sizeof(int));

    if (!nameList.names || !nameList.counter)  {
        ls_syslog(LOG_ERR, I18N_FUNC_FAIL_M, fname, "realloc");
	for(i=0; i < nameList.listSize; i++)
	    FREEUP(nameList.names[i]);
        FREEUP(nameList.names);
        FREEUP(nameList.counter);
	nameList.listSize=0;
        return NULL;
    }

    return &nameList;
}


char *
lsb_printNameList(struct nameList *nameList, int format)
{
    unsigned long namelen;
    char *buf;
    int buflen;
    static char *namestr;
    unsigned long allocLen=0;
    int i;
    int j;

    if (namestr != NULL)
        FREEUP(namestr);

    allocLen = MAXLINELEN;
    namestr = calloc(allocLen, sizeof(char));

    if (!namestr)
        return NULL;

    for (i = 0; i < nameList->listSize; i++) {

        if (format == PRINT_SHORT_NAMELIST) {
            buf = calloc(MAXLINELEN, sizeof(char));
            if (buf == NULL)
                return NULL;
            sprintf(buf, "%d*%s ", nameList->counter[i], nameList->names[i]);
            buflen = strlen(buf);
        } else if ( format == PRINT_MCPU_HOSTS) {
            buf = calloc(MAXLINELEN, sizeof(char));
            if (!buf)
                return NULL;
            sprintf(buf, "%s %d ", nameList->names[i], nameList->counter[i]);
            buflen = strlen(buf);
        }  else {
            namelen = strlen(nameList->names[i]);
            buflen  = (namelen + 1) * nameList->counter[i] + 1;
            buf = calloc(buflen, sizeof(char));
            if (!buf)
                return NULL;
            for(j = 0; j < nameList->counter[i]; j++)
                sprintf(buf+j*(namelen+1), "%s ", nameList->names[i]);
        }

        if (buflen + strlen(namestr) >= allocLen ) {
            allocLen +=  buflen + MAXLINELEN;
            namestr = realloc(namestr, allocLen * sizeof(char));
            if ( !namestr)
                return NULL;
        }
        strcat(namestr, buf);
	FREEUP(buf);
    }
    return namestr;
}


struct nameList *
lsb_parseLongStr(char *string)
{
    static struct nameList nameList;
    unsigned long numStr = strlen(string)/2 + 1;
    char *prevStr;
    char *curStr;
    unsigned long numSameStr;
    int   i;

    if (nameList.names != NULL) {
         for(i=0; i < nameList.listSize; i++)
             FREEUP(nameList.names[i]);
         FREEUP(nameList.names);
         FREEUP(nameList.counter);
    }

    nameList.listSize = 0;
    nameList.names = calloc(numStr, sizeof(char *));
    nameList.counter = calloc(numStr,  sizeof(int));

    if (!nameList.names || !nameList.counter)  {
        FREEUP(nameList.names);
        FREEUP(nameList.counter);
        return NULL;
    }

    numSameStr = 1;
    prevStr = putstr_(getNextWord_(&string));

    if (strlen(prevStr) <= 0) {
        FREEUP(prevStr);
        FREEUP(nameList.names);
        FREEUP(nameList.counter);
        return &nameList;
    }

    while ((curStr = getNextWord_(&string)) != NULL) {

        if (strcmp(curStr, prevStr)) {

            if (nameList.listSize == numStr)
                return NULL;

            nameList.names[nameList.listSize] = prevStr;
            nameList.counter[nameList.listSize] = numSameStr;
            nameList.listSize++;
            numSameStr = 1;
            prevStr = putstr_(curStr);
        } else {
            numSameStr++;
        }
    }

    nameList.names[nameList.listSize] = prevStr;
    nameList.counter[nameList.listSize] = numSameStr;
    nameList.listSize++;

    nameList.names = realloc(nameList.names,
                             nameList.listSize * sizeof(char *));
    nameList.counter = realloc(nameList.counter,
                               nameList.listSize * sizeof(int));

    if (!nameList.names || !nameList.counter)  {
	for(i = 0; i < nameList.listSize; i++)
	    FREEUP(nameList.names[i]);
        FREEUP(nameList.names);
        FREEUP(nameList.counter);
	nameList.listSize=0;
        return NULL;
    }
    return &nameList;

}


struct nameList *
lsb_parseShortStr(char *string, int format)
{
    static struct nameList nameList;
    unsigned long numStr = strlen(string)/2 + 1;
    unsigned int  numSameStr;
    char namestr[4*MAXLINELEN];
    char *name;
    char *curStr;
    int     i;

    if (string == NULL || strlen(string) <= 0) {
        FREEUP(nameList.names);
        FREEUP(nameList.counter);
        return NULL;
    }

    if (nameList.names != NULL) {
         for(i = 0; i < nameList.listSize; i++)
             free(nameList.names[i]);
         free(nameList.names);
         free(nameList.counter);
    }

    nameList.listSize = 0;
    nameList.names = calloc(numStr, sizeof(char *));
    nameList.counter = calloc(numStr,  sizeof(int));

    if (!nameList.names || !nameList.counter)  {
        FREEUP(nameList.names);
        FREEUP(nameList.counter);
        return NULL;
    }

    curStr = getNextWord_(&string);
    while (curStr != NULL) {

        if (nameList.listSize >= numStr)
            return NULL;

        if (format == PRINT_MCPU_HOSTS) {
            sprintf(namestr, "%s", curStr);
            name = (char *)namestr;
            if ((curStr = getNextWord_(&string)) == NULL) {
                FREEUP(nameList.names);
                FREEUP(nameList.counter);
                return NULL;
            }
            numSameStr = atoi(curStr);
        }
        else
            name = lsb_splitName(curStr, &numSameStr);
        if (name != NULL) {
            nameList.names[nameList.listSize] = putstr_(name);
            nameList.counter[nameList.listSize] = numSameStr;
            nameList.listSize++;
        }

        curStr = getNextWord_(&string);
   }


   nameList.names = realloc(nameList.names,
                            nameList.listSize * sizeof(char *));
   nameList.counter = realloc(nameList.counter,
                              nameList.listSize * sizeof(int));

   if (!nameList.names || !nameList.counter)  {
       for(i = 0; i < nameList.listSize; i++)
           FREEUP(nameList.names[i]);
       FREEUP(nameList.names);
       FREEUP(nameList.counter);
       nameList.listSize=0;
       return NULL;
   }

   return &nameList;
}

char*
getUnixSpoolDir( char *spoolDir )
{
    char* pTemp = NULL;

    if((pTemp = strchr (spoolDir, '|')) != NULL){
        int i = 0;
	*pTemp = '\0';
        while (isspace(*(--pTemp)))
            i++;
	if (i != 0) {
            *(++pTemp) = '\0';
        }
        pTemp = spoolDir;
    } else if ( spoolDir[0] == '/') {
	pTemp = spoolDir;
    } else {
        pTemp = NULL;
    }
    return pTemp;

}

void
jobId64To32(LS_LONG_INT interJobId, int *jobId, int *jobArrElemId)
{
    *jobArrElemId = LSB_ARRAY_IDX(interJobId);
    *jobId = LSB_ARRAY_JOBID(interJobId);
}

void
jobId32To64(LS_LONG_INT *interJobId, int jobId, int jobArrElemId)
{
    *interJobId =  LSB_JOBID(jobId, jobArrElemId) ;
}

int
supportJobNamePattern(char * jobname)
{
    char *p;
    char *q;

   p = jobname;
   while ((p != NULL) &&
          ((q=strchr(p, '*')) != NULL)) {
        q++;
        p = q;


        if (*q == '\0' || *q == '/' || *q == '*')
                continue;
        return -1;
   }

   return 0;
}

