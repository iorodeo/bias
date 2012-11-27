#ifndef BIAS_GUID_FWD_HPP
#define BIAS_GUID_FWD_HPP

#include <memory>
#include <set>
#include <list>

namespace bias
{
    class Guid;
    class GuidCmp;
    class GuidPtrCmp;

    typedef std::list<Guid> GuidList;
    typedef std::shared_ptr<Guid> GuidPtr;
    typedef std::list<GuidPtr> GuidPtrList;
    typedef std::set<Guid, GuidCmp> GuidSet;
    typedef std::set<GuidPtr, GuidPtrCmp> GuidPtrSet;

}

#endif // #ifndef BIAS_GUID_FWD_HPP
