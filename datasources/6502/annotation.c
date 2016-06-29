#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "machine.h"
#include "annotation.h"

struct Annotations Annotations;

int
annotation_init(void)
{
  bzero(&Annotations, sizeof(Annotations));
  return 0;
}

void
annotation_memset(uintptr_t info,
		  annotation_pred pred, annotation_func func)
{
  struct AnnotationDesc *ad = &(Annotations.memAnns);

  while (ad->func && ad->next != NULL) ad = ad->next;

  if (ad->func) {
    ad->next =  (struct AnnotationDesc *)malloc(sizeof(struct AnnotationDesc));
    assert(ad->next);
    ad = ad->next;
    ad->next = NULL;
  }
  ad->info = info;
  ad->pred = pred;
  ad->func = func;
  Annotations.memCnt++;
}

void
annotation_addrset(address a, byte v,
		   annotation_pred pred, annotation_func func)
{
  intptr_t key;
  struct AnnotationDesc *ad;
  union AddrAnnotationInfo *ai;

  key = annotation_addrhash(a);
  ad = &(Annotations.addrAnns[key]);

  while (ad->func && ad->next != NULL) ad = ad->next;
  if (ad->func) {
    ad->next = (struct AnnotationDesc *)malloc(sizeof(struct AnnotationDesc));
    assert(ad->next);
    ad = ad->next;
    ad->next = NULL;
  }
  ai         = (union AddrAnnotationInfo *)&(ad->info);
  ai->raw    = 0;
  ai->info.a = a;
  ai->info.v = v;
  ad->pred   = pred;
  ad->func   = func;
  Annotations.addrCnt++;
}

