// 20200628 JinWooGyeol
// Assignment 3
// customer_manager2.c
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

/*-------------------------------------------------------------------*/
/* Hash Function */
enum {HASH_MULTIPLIER = 65599};
static int hash_function(const char *pcKey, int iBucketCount)
{
  int i;
  unsigned int uiHash = 0U;
  for (i = 0; pcKey[i] != '\0'; i++)
    uiHash = uiHash * (unsigned int)HASH_MULTIPLIER
          + (unsigned int)pcKey[i];
  return (int)(uiHash % (unsigned int)iBucketCount);
}
/*-------------------------------------------------------------------*/
#define INITIAL_BUCKET_SIZE 1024
/* Node(UserInfo) and Table(DB) */
struct UserInfo {
  char *name;                 // customer name
  char *id;                   // customer id
  int purchase;               // purchase amount (> 0)
  struct UserInfo *next_id;   // next Node linked by hash value(id)
  struct UserInfo *prev_id;   // prev Node linked by hash value(id)
  struct UserInfo *next_name; // next Node linked by hash value(name)
  struct UserInfo *prev_name; // prev Node linked by hash value(name)
};

struct DB {
  struct UserInfo **pArray_id;   
  struct UserInfo **pArray_name;
  int curArrSize;             // current array size (max # of elements)
  int NodeNum;                // number of all Nodes
};
/*-------------------------------------------------------------------*/
/* Create Hash Table(DB) */
DB_T
CreateCustomerDB(void)
{
  DB_T d;
  d = (DB_T) calloc(1, sizeof(struct DB));
  if (d == NULL) 
  {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }
  d->curArrSize = INITIAL_BUCKET_SIZE; // start with 1024
  d->pArray_id = (struct UserInfo **)
                calloc(d->curArrSize, sizeof(struct UserInfo *));
  d->pArray_name = (struct UserInfo **)
                calloc(d->curArrSize, sizeof(struct UserInfo*));
  if (d->pArray_id == NULL) 
  {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->curArrSize);   
    free(d);
    return NULL;
  }
  if (d->pArray_name == NULL) 
  {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->curArrSize);
    free(d->pArray_id);   
    free(d);
    return NULL;
  }
  return d;
}
/*-------------------------------------------------------------------*/
/* Destroy DB */
void
DestroyCustomerDB(DB_T d)
{
  if(d==NULL)
  {
    return;
  }
  struct UserInfo *p, *nextp;
  for(int i=0;i<d->curArrSize;i++)
  {
    for(p=d->pArray_id[i];p!=NULL;p=nextp)
    {
      /* free each node and decrease NodeNum */
      d->NodeNum--;
      nextp=p->next_id;
      free(p->id);
      free(p->name);
      p->id = NULL;
      p->name = NULL;
      free(p);
      p = NULL;
    }
  }
  /* fee pArray_id and pArray_name */
  free(d->pArray_id);
  d->pArray_id = NULL;
  free(d->pArray_name);
  d->pArray_name = NULL;
  d->NodeNum = 0;
  d->curArrSize = 0;
  free(d);
  d = NULL;
}
/*-------------------------------------------------------------------*/
/* Expand the Table(DB) size to multiply 2 */
DB_T
ExpandDB(DB_T d)
{
  d->curArrSize = d->curArrSize * 2;
  struct UserInfo **test1, **test2;
  /* realloc d->pArray_id and d->pArray_name */
  test1 = (struct UserInfo **)realloc(d->pArray_id, 
          (d->curArrSize)*sizeof(struct UserInfo *));
  test2 = (struct UserInfo **)realloc(d->pArray_name, 
          (d->curArrSize)*sizeof(struct UserInfo *));
  /* check test1 and test2 is NULL */
  if (test1 == NULL)
  {
    /* if realloc fail, return NULL -> register function return -1 */
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	  d->curArrSize);
    d->curArrSize = d->curArrSize/2;
    return NULL;
  }
  else if (test2 == NULL)
  {
    /* if realloc of test1 success but test2 fails */
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	  d->curArrSize);
    free(test1);
    d->curArrSize = d->curArrSize/2;
    return NULL;
  }
  else
  {
    /* realloc success */
    d->pArray_id = test1;
    d->pArray_name = test2;
  }
  /* initialize realloc to NULL */
  for(int i = (d->curArrSize/2);i < d->curArrSize;i++)
  {
    d->pArray_id[i] = NULL;
    d->pArray_name[i] = NULL;
  }
  /* reset NodeNum and register again */
  d->NodeNum = 0;
  /* for all Nodes, check hash value is changed or not */
  for(int i = 0;i < (d->curArrSize/2);i++)
  {
    struct UserInfo *p, *nexp;
    for(p = d->pArray_id[i];p != NULL;p = nexp)
    {
      /* new hash and old hash for compare */
      int old_h_id = hash_function(p->id, d->curArrSize/2);
      int new_h_id = hash_function(p->id, d->curArrSize);
      int old_h_name = hash_function(p->name, d->curArrSize/2);
      int new_h_name = hash_function(p->name, d->curArrSize);
      if((old_h_id!=new_h_id) || (old_h_name!=new_h_name))
      {
        /* save id, name, purchase */
        /* delete Node if hash value is changed */
        char *Save_id = strdup(p->id);
        if (Save_id == NULL)
        {
          return NULL;
        }
        char *Save_name = strdup(p->name);
        if (Save_name == NULL)
        {
          free(Save_id);
          Save_id = NULL;
          return NULL;
        }
        int Save_purchase = p->purchase;
        /* check p is first Node or not */
        if(p->prev_id==NULL)
        {
          if(p->prev_name==NULL)
          {
            d->pArray_id[i]=p->next_id;
            d->pArray_name[old_h_name]=p->next_name;
            if(p->next_id)
            {
              p->next_id->prev_id = NULL;
            }
            if(p->next_name)
            {
              p->next_name->prev_name = NULL;
            }
            nexp = p->next_id;
            free(p->id);
            p->id = NULL;
            free(p->name);
            p->name = NULL;
            p->purchase = 0;
            free(p);
            p=NULL;
          }
          else
          {
            d->pArray_id[i]=p->next_id;
            p->prev_name->next_name=p->next_name;
            if(p->next_id)
            {
              p->next_id->prev_id = NULL;
            }
            if(p->next_name)
            {
              p->next_name->prev_name = p->prev_name;
            }
            nexp = p->next_id;
            free(p->id);
            p->id = NULL;
            free(p->name);
            p->name = NULL;
            p->purchase = 0;
            free(p);
            p=NULL;
          }
        }
        else
        {
          if(p->prev_name==NULL)
          {
            p->prev_id->next_id=p->next_id;
            if(p->next_id)
            {
              p->next_id->prev_id=p->prev_id;
            }
            d->pArray_name[old_h_name] = p->next_name;
            if(p->next_name)
            {
              p->next_name->prev_name=NULL;
            }
            nexp = p->next_id;
            free(p->id);
            p->id = NULL;
            free(p->name);
            p->name = NULL;
            p->purchase=0;
            free(p);
            p=NULL;
          }
          else
          {
            p->prev_id->next_id=p->next_id;
            p->prev_name->next_name=p->next_name;
            if(p->next_id)
            {
              p->next_id->prev_id = p->prev_id;
            }
            if(p->next_name)
            {
              p->next_name->prev_name = p->prev_name;
            }
            nexp = p->next_id;
            free(p->id);
            p->id = NULL;
            free(p->name);
            p->name = NULL;
            p->purchase=0;
            free(p);
            p=NULL;
          }
        }
        /* register again to new table */
        RegisterCustomer(d, Save_id, Save_name, Save_purchase);
        /* free Save value */
        free(Save_id);
        Save_id = NULL;
        free(Save_name);
        Save_name = NULL;
        Save_purchase = 0;
      }
      else
      {
        /* same hash value -> not register again */
        nexp=p->next_id;
        /* just increase Nodenum(value is not changed) */
        d->NodeNum++;
      }
    }
  }
  return d;
}
/*-------------------------------------------------------------------*/
/* register id, name, purchase to UserInfo and connect linked list */
int
RegisterCustomer(DB_T d, const char *id,
		 const char *name, const int purchase)
{
  /* check d, id, name and purchase */
  if ((d == NULL)||(id == NULL)||(name == NULL)||(purchase <= 0))
  {
    return (-1);
  }
  /* pointer p, q to check same id or name */
  struct UserInfo *p, *q;
  int h_id = hash_function(id, d->curArrSize);
  int h_name = hash_function(name, d->curArrSize);
  for (p=d->pArray_id[h_id];p!=NULL;p = p->next_id)
  {
    if(strcmp(p->id, id)==0)
    {
      return (-1);
    }
  }
  for (q=d->pArray_name[h_name];q!=NULL;q=q->next_name)
  {
    if(strcmp(q->name, name)==0)
    {
      return (-1);
    }
  }

  /* check to expansion + do not expand after 1048576 */
  if(((d->NodeNum)>=((0.75)*d->curArrSize))&&(d->curArrSize<1048576))
  {
    d=ExpandDB(d);
    if (d == NULL)
    {
      return (-1);
    }
    else
    {
      return(RegisterCustomer(d, id, name, purchase));
    }
  }

  /* make UserInfo and check NULL */
  struct UserInfo *Info = (struct UserInfo *)
                  calloc(1, sizeof(struct UserInfo));
  if(Info == NULL)
  {
    return (-1);
    fprintf(stderr, "Can't allocate a memory for UserInfo\n");
  }
  Info->id = strdup(id);
  if(Info->id==NULL)
  {
    free(Info);
    return (-1);
  }
  Info->name = strdup(name);
  if(Info->name==NULL)
  {
    free(Info->id);
    free(Info);
    return (-1);
  }
  Info->purchase = purchase;

  /* if Info is first Node in pArray_id[h_id] */
  if(d->pArray_id[h_id]==NULL)
  { 
    d->pArray_id[h_id] = Info;
    Info->next_id=NULL;
    Info->prev_id=NULL;
  }
  else
  {
    d->pArray_id[h_id]->prev_id = Info;
    Info->next_id = d->pArray_id[h_id];
    d->pArray_id[h_id] = Info;
    Info->prev_id=NULL;
  }
  /* if Info is fisrt Node in pArray_name[h_name] */
  if(d->pArray_name[h_name]==NULL)
  {
    d->pArray_name[h_name] = Info;
    Info->next_name=NULL;
    Info->prev_name=NULL;
  }
  else
  {
    d->pArray_name[h_name]->prev_name = Info;
    Info->next_name = d->pArray_name[h_name];
    d->pArray_name[h_name] = Info;
    Info->prev_name=NULL;
  }
  /* increase NodeNum */
  d->NodeNum++;
  return 0;
}
/*-------------------------------------------------------------------*/
/* unregister UserInfo by id and return 0 / else -1 */
int
UnregisterCustomerByID(DB_T d, const char *id)
{
  if ((d == NULL) || (id == NULL))
  {
    return (-1);
  }
  int h_id = hash_function(id, d->curArrSize);
  struct UserInfo *p;
  for(p = d->pArray_id[h_id];p != NULL;p = p->next_id)
  {
    if(strcmp(p->id, id) == 0)
    {
      /* hash value by name */
      int h_name = hash_function(p->name, d->curArrSize);
      /* remove in pArray_id */
      if(p->prev_id == NULL)
      {
        d->pArray_id[h_id] = p->next_id;
        if(p->next_id)
        {
          p->next_id->prev_id = NULL;
        }
      }
      else
      {
        p->prev_id->next_id = p->next_id;
        if(p->next_id)
        {
          p->next_id->prev_id = p->prev_id;
        }
      }
      /* remove in pArray_name */
      if(p->prev_name == NULL)
      {
        d->pArray_name[h_name] = p->next_name;
        if(p->next_name)
        {
          p->next_name->prev_name = NULL;
        }
      }
      else
      {
        p->prev_name->next_name = p->next_name;
        if(p->next_name)
        {
          p->next_name->prev_name = p->prev_name;
        }
      }
      /* decrease NodeNum */
      d->NodeNum--;
      free(p->id);
      p->id = NULL;
      free(p->name);
      p->name = NULL;
      p->purchase = 0;
      free(p);
      p=NULL;
      return 0;
    }
  }
  return (-1);
}

/*-------------------------------------------------------------------*/
/* unregister UserInfo by name and return 0 / else -1 */
int
UnregisterCustomerByName(DB_T d, const char *name)
{
  if ((d == NULL) || (name == NULL))
  {
    return (-1);
  }
  int h_name = hash_function(name, d->curArrSize);
  /* if id matches at the first Node */
  struct UserInfo *p;
  for(p=d->pArray_name[h_name];p!=NULL;p=p->next_name)
  {
    if(strcmp(p->name, name)==0)
    {
      /* hash value by id */
      int h_id = hash_function(p->id, d->curArrSize);
      /* remove in pArray_id */
      if(p->prev_id==NULL)
      {
        d->pArray_id[h_id]=p->next_id;
        if(p->next_id)
        {
          p->next_id->prev_id=NULL;
        }
      }
      else
      {
        p->prev_id->next_id=p->next_id;
        if(p->next_id)
        {
          p->next_id->prev_id=p->prev_id;
        }
      }
      /* remove in pArray_name */
      if(p->prev_name==NULL)
      {
        d->pArray_name[h_name]=p->next_name;
        if(p->next_name)
        {
          p->next_name->prev_name=NULL;
        }  
      }
      else
      {
        p->prev_name->next_name=p->next_name;
        if(p->next_name)
        {
          p->next_name->prev_name=p->prev_name;
        }
      }
      /* decrease NodeNum */
      d->NodeNum--;
      free(p->id);
      p->id = NULL;
      free(p->name);
      p->name = NULL;
      p->purchase=0;
      free(p);
      p=NULL;
      return 0;
    }
  }
  return (-1);
}
/*-------------------------------------------------------------------*/
/* find UserInfo with id and return purchase / else -1 */
int
GetPurchaseByID(DB_T d, const char* id)
{
  /* check d and id */
  if ((d == NULL) || (id == NULL))
  {
    return (-1);
  }
  struct UserInfo *p;
  int h_id = hash_function(id, d->curArrSize);
  for (p=d->pArray_id[h_id];p!=NULL;p = p->next_id)
  {
    if(strcmp(p->id, id)==0)
    {
      return p->purchase;
    }
  }
  return (-1);
}
/*-------------------------------------------------------------------*/
/* find UserInfo with name and return purchase / else -1 */
int
GetPurchaseByName(DB_T d, const char* name)
{
  /* check d and name */
  if ((d == NULL) || (name == NULL))
  {
    return (-1);
  }
  struct UserInfo *p;
  int h_name = hash_function(name, d->curArrSize);
  for (p=d->pArray_name[h_name];p!=NULL;p = p->next_name)
  {
    if(strcmp(p->name, name)==0)
    {
      return p->purchase;
    }
  }
  return (-1);
}
/*--------------------------------------------------------------------*/
/* sum the value of function fp and return sum / else -1 */
int
GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  if (d == NULL || fp == NULL)
  {
    return (-1);
  }
  int sum = 0;
  struct UserInfo *p;
  for (int i=0;i<d->curArrSize;i++)
  {
    for(p=d->pArray_id[i];p!=NULL;p=p->next_id)
    {
      sum=sum+fp(p->id, p->name, p->purchase);
    }
  }
  return sum;
}