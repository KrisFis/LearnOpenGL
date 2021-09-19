
#pragma once

#include "IntegerTypes.h"
#include <assert.h>
#include <type_traits>

template<typename T>
class TSharedPtr;

template<typename T>
class TWeakPtr;

namespace NSharedObject_Private 
{
	class FSharedReferencer
	{
	public:
	
		inline FSharedReferencer()
			: SharedNum(0)
			, WeakNum(0)
		{}
	
	public:
	
		inline void AddShared() { ++SharedNum; }
		inline void AddWeak() { ++WeakNum; }
		
		inline void RemoveShared() 
		{
	#if DEBUG
		assert(SharedNum > 0);
	#endif
		--SharedNum;
		}
		
		inline void RemoveWeak() 
		{
	#if DEBUG
			assert(WeakNum > 0);
	#endif
			--WeakNum;
		}
		
	public: 
	
		inline bool HasSharedReference() const { return SharedNum > 0; }
		inline bool HasWeakReference() const { return WeakNum > 0; }
		inline bool HasAnyReference() const { return (SharedNum + WeakNum) > 0; }
	
	private:
	
		uint16 SharedNum;
		uint16 WeakNum;
	};
	
	struct FNullType {};
	
	template<typename T, typename R>
	struct TSharedMake
	{
	public:
		using ObjectType = T;
		using ReferencerType = R;
	};
	
	template<typename T, typename R>
	struct TSharedCast
	{ 
	
	public:
	
		using FromType = T;
		using ToType = R;
	
		enum 
		{
			IsCastable = std::is_convertible<FromType, ToType>::value,
			IsSame = std::is_same<FromType, ToType>::value
		};
	};
}

template<typename T>
class TSharedPtr
{

public: // Typedefs

	typedef T ObjectType;
	
public: // Constructor
	
	inline TSharedPtr(NSharedObject_Private::FNullType* = nullptr)
		: Object(nullptr)
		, Referencer(nullptr)
	{}
	
	template<typename WeakType, typename = typename std::enable_if<std::is_same_v<WeakType, TWeakPtr<ObjectType>>>>
	inline explicit TSharedPtr(const WeakType& WeakPtr)
		: Object(WeakPtr.Object)
		, Referencer(WeakPtr.Referencer)
	{
		if(Referencer)
			Referencer->AddShared();
	}
	
public: // Copy/Move constructors [SharedPtr]
	
	inline TSharedPtr(const TSharedPtr& Other)
		: Object(Other.Object)
		, Referencer(Other.Referencer)
	{
		if(Referencer)
			Referencer->AddShared();
	}
	
	inline TSharedPtr(TSharedPtr&& Other)
		: Object(Other.Object)
		, Referencer(Other.Referencer)
	{
		Other.Object = nullptr;
		Other.Referencer = nullptr;
	}
	
public: // Destructor
	
	inline ~TSharedPtr()
	{
		RemoveReference();
	}
	
public: // Operators

	inline bool operator==(const TSharedPtr& Other) const { return Object == Other.Object;}
	inline bool operator!=(const TSharedPtr& Other) const { return !operator==(Other); }

	inline void operator=(const TSharedPtr& Other) { RemoveReference(); Object = Other.Object; Referencer = Other.Referencer; if(Referencer) Referencer->AddShared(); }
	inline void operator=(TSharedPtr&& Other) { RemoveReference(); Object = Other.Object; Referencer = Other.Referencer; Other.Object = nullptr; Other.Referencer = nullptr; }
	inline void operator=(NSharedObject_Private::FNullType*) { RemoveReference(); }

	inline const ObjectType* operator->() const { return Get(); }
	inline ObjectType* operator->() { return Get(); }

	inline const ObjectType& operator*() const 
	{
#if DEBUG
		assert(IsValid());
#endif
		return *Get(); 
	}
	
	inline ObjectType& operator*()
	{
#if DEBUG
		assert(IsValid());
#endif
		return *Get(); 
	}

public: // Validation

	inline bool IsValid() const { return (Referencer) ? Referencer->HasSharedReference() : false; }

public: // External methods

	inline void Reset()
	{
		assert(IsValid());
		RemoveReference();
	}

	inline const ObjectType* Get() const { return Object; }
	inline ObjectType* Get() { return Object; }

private: // Helper methods

	inline void RemoveReference()
	{
		if(!IsValid()) return;
		
		Referencer->RemoveShared();
		if(!Referencer->HasSharedReference())
			delete Object;
		if(!Referencer->HasAnyReference())
			delete Referencer;
			
		Object = nullptr;
		Referencer = nullptr;
	}
	
public: // Internal static methods

	template<typename MakeType, typename... ArgTypes>
	inline static TSharedPtr<typename MakeType::ObjectType> Internal_Make(ArgTypes&&... Args)
	{
		TSharedPtr<typename MakeType::ObjectType> resultPtr;
		resultPtr.Object = new typename MakeType::ObjectType(std::forward<ArgTypes>(Args)...);
		resultPtr.Referencer = new typename MakeType::ReferencerType();
		resultPtr.Referencer->AddShared();
		
		return resultPtr;
	}
	
	template<typename MakeType>
	inline static TSharedPtr<typename MakeType::ObjectType> Internal_Make(typename MakeType::ObjectType* Instance)
	{
		TSharedPtr<typename MakeType::ObjectType> resultPtr;
		resultPtr.Object = Instance;
		resultPtr.Referencer = new typename MakeType::ReferencerType();
		resultPtr.Referencer->AddShared();
		
		return resultPtr;
	}
	
	template<typename CastType, typename = typename std::enable_if<CastType::IsCastable>>
	inline static TSharedPtr<typename CastType::ToType> Internal_Cast(const TSharedPtr<typename CastType::FromType>& From)
	{
		static_assert(!CastType::IsSame, "Invalid cast to same type");
	
		TSharedPtr<typename CastType::ToType> resultPtr;
		resultPtr.Object = static_cast<typename CastType::ToType*>(From.Object);
		resultPtr.Referencer = From.Referencer;
		resultPtr.Referencer->AddShared();
		
		return resultPtr;
	}

private: // Fields

	ObjectType* Object;
	NSharedObject_Private::FSharedReferencer* Referencer;

private: // Friends

	template<typename OtherT> friend class TSharedPtr;
	template<typename OtherT> friend class TWeakPtr;
};

template<typename T>
class TWeakPtr
{
	
public: // Typedefs

	typedef T ObjectType;

public: // Constructors

	inline TWeakPtr(NSharedObject_Private::FNullType* = nullptr)
		: Object(nullptr)
		, Referencer(nullptr)
	{}
	
	template<typename SharedType, typename = typename std::enable_if<std::is_same_v<SharedType, TSharedPtr<ObjectType>>>>
	inline explicit TWeakPtr(const SharedType& SharedPtr)
		: Object(SharedPtr.Object)
		, Referencer(SharedPtr.Referencer)
	{
		if(Referencer)
			Referencer->AddWeak();
	}

public: // Copy/Move constructors [WeakPtr]

	inline TWeakPtr(const TWeakPtr& Other)
		: Object(Other.Object)
		, Referencer(Other.Referencer)
	{
		if(Referencer)
			Referencer->AddWeak();
	}
	
	inline TWeakPtr(TWeakPtr&& Other)
		: Object(Other.Object)
		, Referencer(Other.Referencer)
	{
		Other.Object = nullptr;
		Other.Referencer = nullptr;
	}
	
public: // Destructor

	inline ~TWeakPtr()
	{
		RemoveReference();
	}

public: // Operators

	inline bool operator==(const TWeakPtr& Other) const { return Object == Other.Object; }
	inline bool operator!=(const TWeakPtr& Other) const { return !operator==(Other); }

	inline void operator=(const TWeakPtr& Other) { RemoveReference(); Object = Other.Object; Referencer = Other.Referencer; if(Referencer) Referencer->AddWeak(); }
	inline void operator=(TWeakPtr&& Other) { RemoveReference(); Object = Other.Object; Referencer = Other.Referencer; Other.Object = nullptr; Other.Referencer = nullptr; }

public: // Validity

	inline bool IsValid() const { return (Referencer) ? Referencer->HasSharedReference() : false; }

public: // Methods

	inline void Reset()
	{
		RemoveReference();
	}

	inline TSharedPtr<ObjectType> Pin() const
	{
		if(!IsValid()) return nullptr;
		return TSharedPtr<ObjectType>(*this);
	}

private: // Internal methods

	inline void RemoveReference()
	{
		if(!IsValid()) return;
		
		Referencer->RemoveWeak();
		if(!Referencer->HasAnyReference())
			delete Referencer;
	}

private: // Fields

	ObjectType* Object;
	NSharedObject_Private::FSharedReferencer* Referencer;

private: // Friend class

	template<typename OtherT> friend class TSharedPtr;
	template<typename OtherT> friend class TWeakPtr;
};

//////////////////////////////////////////////////
// Utils
//////////////////////////////////////////////////

template<typename T, typename... ArgTypes>
inline TSharedPtr<T> MakeShared(ArgTypes&&... Args)
{
	return TSharedPtr<T>::template Internal_Make<NSharedObject_Private::TSharedMake<T, NSharedObject_Private::FSharedReferencer>>(std::forward<ArgTypes>(Args)...);
}

template<typename T>
inline TSharedPtr<T> MakeShareable(T* Instance)
{
	return TSharedPtr<T>::template Internal_Make<NSharedObject_Private::TSharedMake<T, NSharedObject_Private::FSharedReferencer>>(Instance);
}

template<typename T, typename R>
inline TSharedPtr<T> Cast(const TSharedPtr<R>& From)
{
	return TSharedPtr<R>::template Internal_Cast<NSharedObject_Private::TSharedCast<R, T>>(From);
}