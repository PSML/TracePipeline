#ifndef __ANNOTATION_H__
#define __ANNOTATION_H__

struct AnnotationDesc;

typedef int (*annotation_pred)(struct AnnotationDesc *ad);
typedef int (*annotation_func)(struct AnnotationDesc *ad);

union AddrAnnotationInfo {
  uintptr_t raw;
  struct {
    address a;
    byte v;
  } info;
};

struct AnnotationDesc {
  uintptr_t              info;
  annotation_pred        pred;
  annotation_func        func;
  struct AnnotationDesc *next;
};

struct Annotations {
  struct AnnotationDesc addrAnns[1<<16];
  struct AnnotationDesc memAnns;
  int addrCnt;
  int memCnt;
  int addrDone;
  int memDone;
};
extern struct Annotations Annotations;

inline static void
annotation_do(struct AnnotationDesc *ad)
{
  if (ad->pred) {
    if (ad->pred(ad)) ad->func(ad);
  }
  else if (ad->func) ad->func(ad);
}

inline static intptr_t
annotation_addrhash(address a)
{
  intptr_t offset = a;
  return offset;
}

inline static intptr_t
annotation_addrhash_machine()
{
  if (ERTRACE_MEMCTR() & MEM_RD8_CTR) {
    return annotation_addrhash(MEM_TYPE(memRdWr, memRdAddr));
  }
  if (ERTRACE_MEMCTR() & MEM_WR8_CTR) {
    return annotation_addrhash(MEM_TYPE(memRdWr, memWrAddr));
  } else if (ERTRACE_MEMCTR() & MEM_WR16_CTR) {
    return annotation_addrhash(MEM_TYPE(memWr16, memWrAddr));
  } else if (ERTRACE_MEMCTR() & MEM_WR24_CTR) {
    return annotation_addrhash(MEM_TYPE(memWr24, memWrAddr));
  }
  return -1;
}

inline static void annotation_process(void)
{
  if (Annotations.memCnt) {
    Annotations.memDone++;
    struct AnnotationDesc *ad;
    for (ad = &(Annotations.memAnns); ad != NULL; ad=ad->next) {
      annotation_do(ad);
    }
  }
  if (Annotations.addrCnt) {
    Annotations.addrDone++;
    struct AnnotationDesc *ad;
    intptr_t key = annotation_addrhash_machine();
    if (key < 0) return;
    for (ad = &(Annotations.addrAnns[key]); ad != NULL; ad=ad->next) {
      annotation_do(ad);
    }
  }
}


extern int  annotation_init(void);
extern void annotation_addrset(address a, byte v,
			      annotation_pred pred, annotation_func func);
extern void annotation_memset(uintptr_t info, 
			      annotation_pred pred, annotation_func func);
#endif
