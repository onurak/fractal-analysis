/* Author: Alexander Shargin and Vladimir Belikov
 * Originally it was posted in RSDN Magazine #2
 * (http://www.rsdn.ru/article/cpp/delegates.xml).
 *
 * This file is part of G Library.
 *
 * G Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * G Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with G Libray.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <list>

/*! \file
  * \brief Delegates realization from RSDN
  */

#define COMBINE_(a,b)     a##b
#define COMBINE(a,b)      COMBINE_(a,b)

#define GI_DELEGATE       COMBINE(GIDelegate, SUFFIX)
#define G_STATIC_DELEGATE COMBINE(GStaticDelegate, SUFFIX)
#define G_METHOD_DELEGATE COMBINE(GMethodDelegate, SUFFIX)
#define G_DELEGATE        COMBINE(GDelegate, SUFFIX)

/*! \class GIDelegateX
  * \brief Base interface for single delegate.
  */
template <class TRet TEMPLATE_PARAMS>
class GI_DELEGATE
{
public:
    virtual ~GI_DELEGATE() {}
    virtual TRet invoke(PARAMS) = 0;
    virtual bool comapre(GI_DELEGATE<TRet TEMPLATE_ARGS> *pDelegate) = 0;
};

/*! \class GStaticDelegateX
  * \brief Delegate for simple non-member function or static member function.
  */
template <class TRet TEMPLATE_PARAMS>
class G_STATIC_DELEGATE: public GI_DELEGATE<TRet TEMPLATE_ARGS>
{
public:
    typedef TRet (*PFunc)(PARAMS);    
    G_STATIC_DELEGATE(PFunc pFunc) { m_pFunc = pFunc; }

    virtual TRet invoke(PARAMS) { return m_pFunc(ARGS); }

    virtual bool comapre(GI_DELEGATE<TRet TEMPLATE_ARGS> *pDelegate) {
        G_STATIC_DELEGATE<TRet TEMPLATE_ARGS> *pStaticDel =
                dynamic_cast<G_STATIC_DELEGATE<TRet TEMPLATE_ARGS>*>(pDelegate);
        if (pStaticDel == NULL || pStaticDel->m_pFunc != m_pFunc)
            return false;
        return true;
    }
private:
    PFunc m_pFunc;
};

/*! \class GMethodDelegateX
  * \brief Delegate for member function
  */
template <class TObj, class TRet TEMPLATE_PARAMS>
class G_METHOD_DELEGATE: public GI_DELEGATE<TRet TEMPLATE_ARGS>
{
public:
    typedef TRet (TObj::*PMethod)(PARAMS);

    G_METHOD_DELEGATE(TObj *pObj, PMethod pMethod) {
        m_pObj = pObj;
        m_pMethod = pMethod;
    }

    virtual TRet invoke(PARAMS) {
        return (m_pObj->*m_pMethod)(ARGS);
    }

    virtual bool comapre(GI_DELEGATE<TRet TEMPLATE_ARGS> *pDelegate) {
        G_METHOD_DELEGATE<TObj, TRet TEMPLATE_ARGS> *pMethodDel =
                dynamic_cast<G_METHOD_DELEGATE<TObj, TRet TEMPLATE_ARGS>*>(pDelegate);
        if (pMethodDel == NULL || pMethodDel->m_pMethod != m_pMethod || pMethodDel->m_pObj != m_pObj)
            return false;
        return true;
    }

private:
    TObj *m_pObj;
    PMethod m_pMethod;
};

//! Function that create static function delegate
template <class TRet TEMPLATE_PARAMS>
GI_DELEGATE<TRet TEMPLATE_ARGS>* delegate(TRet (*pFunc)(PARAMS))
{
    return new G_STATIC_DELEGATE<TRet TEMPLATE_ARGS>(pFunc);
}

//! Function that create member-function delegate
template <class TObj, class TRet TEMPLATE_PARAMS>
GI_DELEGATE<TRet TEMPLATE_ARGS>* delegate(TObj *pObj, TRet (TObj::*pMethod)(PARAMS))
{
    return new G_METHOD_DELEGATE<TObj, TRet TEMPLATE_ARGS>(pObj, pMethod);
}

/*! \class GDelegateX
  * \brief Generic delegate (contains list of single delegates)
  *
  * It holds list of single delegates, allow to invoke them at once.
  * Syntax is C#-like.
  */
template <class TRet TEMPLATE_PARAMS>
class G_DELEGATE
{
public:
    typedef GI_DELEGATE<TRet TEMPLATE_ARGS> GIDelegate;
    typedef std::list<GIDelegate*> DelegateList;

    G_DELEGATE(GIDelegate *pDelegate = NULL) {
        removeAll();
        if (pDelegate)
            add(pDelegate);
    }

    ~G_DELEGATE() {
        removeAll();
    }

    bool isNull() { return m_delegateList.empty(); }

    G_DELEGATE<TRet TEMPLATE_ARGS>& operator=(GIDelegate *pDelegate)
    {
        removeAll();
        add(pDelegate);
        return *this;
    }

    G_DELEGATE<TRet TEMPLATE_ARGS>& operator+=(GIDelegate *pDelegate)
    {
        add(pDelegate);
        return *this;
    }

    G_DELEGATE<TRet TEMPLATE_ARGS>& operator-=(GIDelegate *pDelegate)
    {
        remove(pDelegate);
        return *this;
    }

    TRet operator()(PARAMS)
    {
        return invoke(ARGS);
    }

private:
    void add(GIDelegate *pDelegate)
    {
        if (pDelegate)
            m_delegateList.push_back(pDelegate);
    }

    void remove(GIDelegate *pDelegate)
    {
        typename DelegateList::iterator p;
        for (p = m_delegateList.begin(); p != m_delegateList.end(); ++p) {
            if ((*p)->compare(pDelegate)) {
                delete *p;
                m_delegateList.erase(p);
                break;
            }
        }
    }

    void removeAll()
    {
        typename DelegateList::iterator p;
        for (p = m_delegateList.begin(); p != m_delegateList.end(); ++p)
            delete *p;
        m_delegateList.clear();
    }


    //#pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wreturn-type"
    TRet invoke(PARAMS)
    {
        if (!isNull())
        {
            typename DelegateList::const_iterator p;
            for (p = m_delegateList.begin(); p != --m_delegateList.end(); ++p)
                (*p)->invoke(ARGS);

            return m_delegateList.back()->invoke(ARGS);
        }
    }
    //#pragma GCC diagnostic pop

private:
    DelegateList m_delegateList;
};

