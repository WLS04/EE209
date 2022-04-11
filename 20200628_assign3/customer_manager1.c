// 20200628 JinWooGyeol
// Assignment 3
// customer_manager1.c
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customer_manager.h"

/*-------------------------------------------------------------------*/
#define UNIT_ARRAY_SIZE 1024
struct UserInfo {
  char *name;               // customer name
  char *id;                 // customer id
  int purchase;             // purchase amount (> 0)
};

struct DB {
  struct UserInfo *pArray;  // pointer to the array
  int curArrSize;           // current array size (max # of elements)
  int numItems;             // # of stored items, needed to determine
};
/*-------------------------------------------------------------------*/

/*-------------------------------------------------------------------*/
/* create d, d->pArray, initialize d->curArrSize and return d  */
DB_T
CreateCustomerDB(void)
{
  DB_T d;
  d = (DB_T) calloc(1, sizeof(struct DB));
  if (d == NULL) {
    fprintf(stderr, "Can't allocate a memory for DB_T\n");
    return NULL;
  }
  d->curArrSize = UNIT_ARRAY_SIZE; // start with 1024 elements
  d->numItems = 0;
  d->pArray = (struct UserInfo *)calloc(d->curArrSize,
               sizeof(struct UserInfo));
  if (d->pArray == NULL) {
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	    d->curArrSize);   
    free(d);
    return NULL;
  }
  return d;
}
/*-------------------------------------------------------------------*/
/* free d and name, id inside d->pArray */
void
DestroyCustomerDB(DB_T d)
{
  if (d == NULL)
  {
    return;
  }
  for (int i = 0;i < d->curArrSize;i++)
  {
    if ((d->pArray[i].name) && (d->pArray[i].id))
    {
      /* decrease numItems */
      d->numItems--;
      /* free name and id */
      free(d->pArray[i].name);
      d->pArray[i].name = NULL;
      free(d->pArray[i].id);
      d->pArray[i].id = NULL;
      d->pArray[i].purchase = 0;
    }
  }
  free(d->pArray);
  d->pArray = NULL;
  free(d);
  d = NULL;
}
/*-------------------------------------------------------------------*/
/* register id, name, purchase to d->pArray */
int
RegisterCustomer(DB_T d, const char *id,
		 const char *name, const int purchase)
{
  /* check d, id, name and purchase */
  if ((d == NULL)||(id == NULL)||(name == NULL)||(purchase <= 0))
  {
    return (-1);
  }
  /* check same name or id */
  for (int i = 0;i < d->curArrSize;i++)
  {
    if((d->pArray[i].name) && (d->pArray[i].id))
    {
      if(strcmp(d->pArray[i].id, id) == 0 || 
        strcmp(d->pArray[i].name, name) == 0)
        return (-1);
    }
  }
  /* No same ID or Name -> find empty element and save */
  if((d->curArrSize) > (d->numItems))
  {
    for (int i = 0;i < d->curArrSize;i++)
    {
      if(d->pArray[i].purchase == 0) // purchase == 0 -> empty
      {
        d->pArray[i].id = strdup(id);
        if(d->pArray[i].id == NULL)
        {
          return (-1);
        }
        d->pArray[i].name = strdup(name);
        if(d->pArray[i].name == NULL)
        {
          free(d->pArray[i].id);
          d->pArray[i].id = NULL;
          return (-1);
        }
        d->pArray[i].purchase = purchase;
        /* increase numItems */
        d->numItems++;
        return 0;
      }
    }
  }
  /* if pArray is full, realloc the size */
  d->curArrSize = d->curArrSize + UNIT_ARRAY_SIZE;
  struct UserInfo *p;
  p = (struct UserInfo *)realloc(d->pArray,
      (d->curArrSize)*sizeof(struct UserInfo));
  /* check realloc is correct */
  if (p == NULL) 
  {
    d->curArrSize = d->curArrSize - UNIT_ARRAY_SIZE;
    fprintf(stderr, "Can't allocate a memory for array of size %d\n",
	  d->curArrSize);   
    return (-1);
  }
  else
  {
    d->pArray = p;
  }
  /* initialize realloc */
  for (int i=((d->curArrSize)-UNIT_ARRAY_SIZE);i<d->curArrSize;i++)
  {
    d->pArray[i].id = NULL;
    d->pArray[i].name = NULL;
    d->pArray[i].purchase = 0;
  }
  return RegisterCustomer(d, id, name, purchase);
}
/*-------------------------------------------------------------------*/
/* find element with id and unregister it / else -1 */
int
UnregisterCustomerByID(DB_T d, const char *id)
{
  /* check d, id */
  if ((d == NULL)||(id == NULL))
  {
    return (-1);
  }
  for (int i = 0;i < d->curArrSize;i++)
  {
    if((d->pArray[i].name) && (d->pArray[i].id))
    {
      /* same id */
      if(strcmp(d->pArray[i].id, id) == 0)
      {
        free(d->pArray[i].id);
        free(d->pArray[i].name);
        d->pArray[i].purchase = 0;
        d->pArray[i].id = NULL;
        d->pArray[i].name = NULL;
        /* decrease numItems */
        d->numItems--;
        return 0;
      }
    }
  }
  return (-1);
}

/*-------------------------------------------------------------------*/
/* find element with name and unregister it / else -1 */
int
UnregisterCustomerByName(DB_T d, const char *name)
{
  /* check d, name */
  if((d == NULL)||(name == NULL))
  {
    return (-1);
  }
  for (int i = 0;i < d->curArrSize;i++)
  {
    if((d->pArray[i].name) && (d->pArray[i].id))
    {
      /* same name */
      if(strcmp(d->pArray[i].name, name) == 0)
      {
        free(d->pArray[i].id);
        free(d->pArray[i].name);
        d->pArray[i].purchase = 0;
        d->pArray[i].id = NULL;
        d->pArray[i].name = NULL;
        /* decrease numItems */
        d->numItems--;
        return 0;
      }
    }
  }
  return (-1);
}
/*-------------------------------------------------------------------*/
/* find element with id and return purchase value / else -1 */
int
GetPurchaseByID(DB_T d, const char* id)
{
  /* check d, id */
  if((d == NULL)||(id == NULL))
  {
    return (-1);
  }
  for (int i = 0;i < d->curArrSize;i++)
  {
    if((d->pArray[i].name) && (d->pArray[i].id))
    {
      /* same id */
      if(strcmp(d->pArray[i].id, id) == 0)
      {
        return d->pArray[i].purchase;
      }
    }
  }
  return (-1);
}
/*-------------------------------------------------------------------*/
/* find element with name and return purchase value / else -1 */
int
GetPurchaseByName(DB_T d, const char* name)
{
  /* check d, name */
  if((d == NULL)||(name == NULL))
  {
    return (-1);
  }
  for (int i = 0;i < d->curArrSize;i++)
  {
    if((d->pArray[i].name) && (d->pArray[i].id))
    {
      /* same name */
      if(strcmp(d->pArray[i].name, name) == 0)
      {
        return d->pArray[i].purchase;
      }
    }
  }
  return (-1);
}
/*-------------------------------------------------------------------*/
/* sum the value of function fp and return sum / else -1 */
int
GetSumCustomerPurchase(DB_T d, FUNCPTR_T fp)
{
  /* check d, fp */
  if ((d == NULL)||(fp == NULL))
  {
    return (-1);
  }
  int sum = 0;
  for (int i=0;i<d->curArrSize;i++)
  {
    /* valid element */
    if(d->pArray[i].name && d->pArray[i].id)
    {
      sum = sum+fp(d->pArray[i].id, d->pArray[i].name, d->pArray[i].purchase);
    }
  }
  return sum;
}