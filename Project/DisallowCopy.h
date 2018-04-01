#ifndef _CG_DISALLOW_COPY_
#define _CG_DISALLOW_COPY_

// This class prevents a class from automatically creating an assignment operator. This
// is dangerous with pointer members so by inheriting from this class this copying
// prevented.

class DisallowCopy
{
public:
	DisallowCopy() {};
private:
	DisallowCopy(const DisallowCopy &rhs) {};
	DisallowCopy & operator=(const DisallowCopy &rhs) {};
};

#endif