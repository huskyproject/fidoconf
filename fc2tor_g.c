/*--------------------------------------------------
    Victor 'mgl' Anikeev, mgl@pisem.net
--------------------------------------------------*/

/*#define MGLGRPTST*/

#ifndef __MGLGROUPS_C__
#define __MGLGROUPS_C__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fidoconf.h"
#include "common.h"

#include "fc2tor_g.h"

int getGroupSetSize(char *str, char *delms) {
 int num = 0;

 if (str == NULL) return 0;
 while (*str) {
  while ((*str) && strchr(delms, *str)) str++;
  if (*str) {
    num++;
    while ((*str) && !strchr(delms, *str)) str++;
  }
 }
 return num;
}

int isSubSetOfGroups(char *sub, char *groups, char *delms) {
 register char *temp;
 register char *grp;
 int len, ok;

 if (sub == NULL) return 1;
 while (*sub) {
  while ((*sub) && strchr(delms, *sub)) sub++;
  if (*sub) {
    grp = sub; len = 0;
    while ((*sub) && !strchr(delms, *sub)) {
      sub++; len++;
    }
    temp = groups; ok = 0;
    while (!ok && *temp) {
      while (*temp && strchr(delms, *temp)) temp++;
      if (strncmp(grp, temp, len) == 0) ok = 1;
      while (*temp && !strchr(delms, *temp)) temp++;
    }
    if (!ok) return 0;
  }
 }
 return 1;
}

int areCrossGroupSets(char *grp1, char *grp2, char *delms) {
 register char *temp;
 register char *grp;
 int len;

 if (grp1) while ((*grp1) && strchr(delms, *grp1)) grp1++;
 if (grp2) while ((*grp2) && strchr(delms, *grp2)) grp2++;

 if ((!grp1 || !*grp1) && (!grp1 || !*grp1)) return 1;
 while (*grp1) {
  while ((*grp1) && strchr(delms, *grp1)) grp1++;
  if (*grp1) {
    grp = grp1; len = 0;
    while ((*grp1) && !strchr(delms, *grp1)) {
      grp1++; len++;
    }
    temp = grp2;
    while (*temp) {
      while (*temp && strchr(delms, *temp)) temp++;
      if (strncmp(grp, temp, len) == 0) return 1;
      while (*temp && !strchr(delms, *temp)) temp++;
    }
  }
 }
 return 0;
}

int areEqualGroupSets(char *grp1, char *grp2, char *delms) {
 if (!isSubSetOfGroups(grp1, grp2, delms)) return 0;
 if (!isSubSetOfGroups(grp2, grp1, delms)) return 0;
 return 1;
}

/*-----------------25.08.02 00:36-------------------
 for TEST only (MGLGRPTST must be defined)
--------------------------------------------------*/
#ifdef MGLGRPTST
int main (int argc, char *argv[]) {
 printf("Groups: %d\n", getGroupSetSize(",, , ,os/2,,,unix,, ,win,,,", ",; "));
 printf("isSubSet: %d\n", isSubSetOfGroups("quake ,,,doom", "doom;wolf,quake", ", ;"));
 printf("isSubSet: %d\n", isSubSetOfGroups("quake doom", "quake doom", " "));
 printf("isSubSet: %d\n", isSubSetOfGroups("quake", "quake doom", " "));
 printf("isSubSet: %d\n", isSubSetOfGroups("", "quake doom", " "));
 printf("isSubSet: %d\n", isSubSetOfGroups("quake doom", "wolf quake", " "));
 printf("isSubSet: %d\n", isSubSetOfGroups("quake", "doom", " "));
 printf("isSubSet: %d\n", isSubSetOfGroups("quake", "", " "));
 printf("\n");
 printf("areCross: %d\n", areCrossGroupSets("quake", "doom", " "));
 printf("areCross: %d\n", areCrossGroupSets("quake", "", " "));
 printf("areCross: %d\n", areCrossGroupSets("quake", "quake", " "));
 printf("areCross: %d\n", areCrossGroupSets("quake doom", "doom wolf", " "));
 printf("\n");
 printf("areEqual: %d\n", areEqualGroupSets("", "", " "));
 printf("areEqual: %d\n", areEqualGroupSets("doom", "quake", " "));
 printf("areEqual: %d\n", areEqualGroupSets("doom wolf", "wolf doom", " "));
 printf("\n");
 return 0;
}
#endif

#endif
