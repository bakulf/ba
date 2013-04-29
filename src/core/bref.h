#ifndef _BA_REF_H_
#define _BA_REF_H_

#include <QObject>

#include <assert.h>

template <class T>
class BRef
{
public:
  BRef()
  : mRawPtr(0)
  {
  }

  BRef(const BRef& aCopy)
  : mRawPtr(aCopy.mRawPtr)
  {
    if (mRawPtr) {
      mRawPtr->addRef();
    }
  }

  BRef(T* aRawPtr)
  : mRawPtr(aRawPtr)
  {
    if (mRawPtr) {
      mRawPtr->addRef();
    }
  }

  virtual ~BRef()
  {
    if (mRawPtr) {
      mRawPtr->release();
    }
  }

  BRef&
  operator=(const BRef& aCopy)
  {
    assignWithAddRef(aCopy.mRawPtr);
    return *this;
  }

  BRef&
  operator=(T* aRawPtr)
  {
    assignWithAddRef(aRawPtr);
    return *this;
  }

  T*
  get() const
  {
    return mRawPtr;
  }

  operator T*() const
  {
    return get();
  }

  T*
  operator->() const
  {
    if (!mRawPtr) {
      assert("You can't dereference a NULL nsRefPtr with operator->().");
    }
    return get();
  }

  T&
  operator*() const
  {
    if (!mRawPtr) {
      assert("You can't dereference a NULL nsRefPtr with operator->().");
    }
    return *get();
  }

private:
  void
  assignWithAddRef(T* aRawPtr)
  {
    if (aRawPtr) {
      aRawPtr->addRef();
    }
    assignAssumingAddRef(aRawPtr);
  }

  void
  assignAssumingAddRef(T* aRawPtr)
  {
    T* oldPtr = mRawPtr;
    mRawPtr = aRawPtr;
    if (oldPtr) {
      oldPtr->release();
    }
  }

private:
  T* mRawPtr;
};

class BRefObj
{
public:
  BRefObj()
  : mRef(0)
  {
  }

  virtual ~BRefObj()
  {
  }

  void addRef()
  {
    ++mRef;
  }

  void release()
  {
    --mRef;
    if (mRef < 0) {
      assert("MRef < 0 ?!?");
    }

    if (!mRef) {
      delete this;
    }
  }

private:
  qint64 mRef;
};

#endif
