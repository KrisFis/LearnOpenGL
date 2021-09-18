
#pragma once

#include "IntegerTypes.h"
#include <assert.h>
#include <type_traits>

template<typename T>
class TSharedPtr;

template<typename T>
class TWeakPtr;

template<typename T, typename... Args>
TSharedPtr<T> MakeShared(Args&&...);

template<typename T>
TSharedPtr<T> MakeShareable(T*);

class FSharedReferencer
{
public:

	FSharedReferencer();

public:

	inline void AddShared() { ++SharedCount; }
	inline void AddWeak() { ++WeakCount; }
	
	inline void RemoveShared() 
	{
#if DEBUG
	assert(SharedCount > 0);
#endif
	--SharedCount;
	}
	
	inline void RemoveWeak() 
	{
#if DEBUG
		assert(WeakCount > 0);
#endif
		--WeakCount;
	}
	
public: 

	inline bool HasSharedReference() const { return SharedCount > 0; }
	inline bool HasWeakReference() const { return WeakCount > 0; }
	inline bool HasAnyReference() const { return (SharedCount + WeakCount) > 0; }

private:

	uint16 SharedCount;
	uint16 WeakCount;
};

template<typename T>
class TSharedPtr
{

public: // Typedefs

	typedef T ObjectType;
	
public: // Constructor
	
	TSharedPtr()
		: Object(nullptr)
		, Referencer(nullptr)
	{}
	
	template<typename PtrType, typename = typename std::enable_if<std::is_null_pointer<PtrType>::Value>>
	TSharedPtr(PtrType*)
		: Object(nullptr)
		, Referencer(nullptr)
	{}
	
public: // Copy/Move constructors [SharedPtr]
	
	TSharedPtr(const TSharedPtr&& Other)
		: Object(Other.Object)
		, Referencer(Other.Referencer)
	{
		if(Referencer)
			Referencer->AddShared();
	}
	
	TSharedPtr(TSharedPtr&& Other)
		: Object(Other.Object)
		, Referencer(Other.Referencer)
	{
		Other.Object = nullptr;
		Other.Referencer = nullptr;
	}
	
public: // Copy/Move constructors [WeakPtr]
		// * explicit

	template<typename WeakType, typename = typename std::enable_if<std::is_same<WeakType, TWeakPtr<ObjectType>>::Value>>
	explicit TSharedPtr(const WeakType& WeakPtr)
		: Object(WeakPtr.Object)
		, Referencer(WeakPtr.Referencer)
	{
		if(Referencer)
			Referencer->AddShared();
	}
	
	template<typename WeakType, typename = typename std::enable_if<std::is_same<WeakType, TWeakPtr<ObjectType>>::Value>>
	explicit TSharedPtr(WeakType&& WeakPtr)
		: Object(WeakPtr.Object)
		, Referencer(WeakPtr.Referencer)
	{
		if(Referencer)
		{
			Referencer->AddShared();
			Referencer->RemoveWeak();
		}
			
		WeakPtr.Object = nullptr;
		WeakPtr.Referencer = nullptr;
	}
	
public: // Destructor
	
	~TSharedPtr()
	{
		RemoveReference();
	}
	
public: // Operators

	bool operator==(const TSharedPtr& Other) const { return Object == Other.Object;}
	bool operator!=(const TSharedPtr& Other) const { return !operator==(Other); }

	void operator=(const TSharedPtr& Other) { RemoveReference(); Object = Other.Object; Referencer = Other.Referencer; if(Referencer) Referencer->AddShared(); }
	void operator=(TSharedPtr&& Other) { RemoveReference(); Object = Other.Object; Referencer = Other.Referencer; Other.Object = nullptr; Other.Referencer = nullptr; }

	const ObjectType* operator->() const { return Get(); }
	ObjectType* operator->() { return Get(); }

	const ObjectType* operator*() const { return Get(); }
	ObjectType* operator*() { return Get(); }

public: // Validation

	inline bool IsValid() const { return (Referencer) ? Referencer->HasSharedReference() : false; }

public: // Getters

	inline const ObjectType* Get() const
	{
#if DEBUG
		assert(IsValid());
#endif
		return Object;
	}
	
	inline ObjectType* Get()
	{
#if DEBUG
		assert(IsValid());
#endif
		return Object;	
	}

protected: // Friend methods

	template<typename ClassType, typename... ArgsType>
	friend TSharedPtr<ClassType> MakeShared(ArgsType&&... Args)
	{
		TSharedPtr<ClassType> resultShared;
		resultShared.Object = new ClassType(std::move(Args...));
		resultShared.Referencer = new FSharedReferencer();
		resultShared.Referencer->AddShared();
		
		return resultShared;
	}

	template<typename ClassType>
	friend TSharedPtr<ClassType> MakeShareable(ClassType* Instance)
	{
		TSharedPtr<ClassType> resultShared(nullptr);
		resultShared.Object = Instance;
		resultShared.Referencer = new FSharedReferencer();
		resultShared.Referencer->AddShared();
		
		return resultShared;
	}

private: // Internal methods

	void RemoveReference()
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
	
private: // Fields

	ObjectType* Object;
	FSharedReferencer* Referencer;

private: // Friend class

	template<typename OtherT> friend class TWeakPtr;
};

template<typename T>
class TWeakPtr
{
	
public: // Typedefs

	typedef T ObjectType;

public: // Constructors

	TWeakPtr()
		: Object(nullptr)
		, Referencer(nullptr)
	{}

	template<typename PtrType, typename = typename std::enable_if<std::is_null_pointer<PtrType>::Value>>
	TWeakPtr(PtrType*)
		: Object(nullptr)
		, Referencer(nullptr)
	{}

public: // Copy/Move constructors [WeakPtr]

	TWeakPtr(const TWeakPtr& Other)
		: Object(Other.Object)
		, Referencer(Other.Referencer)
	{
		if(Referencer)
			Referencer->AddWeak();
	}
	
	TWeakPtr(TWeakPtr&& Other)
		: Object(Other.Object)
		, Referencer(Other.Referencer)
	{
		Other.Object = nullptr;
		Other.Referencer = nullptr;
	}
	
public: // Copy/Move constructors [SharedPtr]
		// * explicit
	
	template<typename SharedType, typename = typename std::enable_if<std::is_same<SharedType, TSharedPtr<ObjectType>>::Value>>
	explicit TWeakPtr(const SharedType& SharedPtr)
		: Object(SharedPtr.Object)
		, Referencer(SharedPtr.Referencer)
	{
		if(Referencer)
			Referencer->AddWeak();
	}
	
	template<typename SharedType, typename = typename std::enable_if<std::is_same<SharedType, TSharedPtr<ObjectType>>::Value>>
	explicit TWeakPtr(SharedType&& SharedPtr)
	{
		SharedPtr.RemoveReference();
		
		Object = SharedPtr.Object;
		Referencer = SharedPtr.Referencer;
		
		if(Referencer)
			Referencer->AddWeak();
	}
	
public: // Destructor

	~TWeakPtr()
	{
		RemoveReference();
	}

public: // Operators

	bool operator==(const TWeakPtr& Other) const { return Object == Other.Object; }
	bool operator!=(const TWeakPtr& Other) const { return !operator==(Other); }

	void operator=(const TWeakPtr& Other) { RemoveReference(); Object = Other.Object; Referencer = Other.Referencer; if(Referencer) Referencer->AddWeak(); }
	void operator=(TWeakPtr&& Other) { RemoveReference(); Object = Other.Object; Referencer = Other.Referencer; Other.Object = nullptr; Other.Referencer = nullptr; }

public: // Validity

	inline bool IsValid() const { return (Referencer) ? Referencer->HasSharedReference() : false; }

public: // Getters

	TSharedPtr<ObjectType> Pin()
	{
		if(!IsValid()) return nullptr;
		return TSharedPtr<ObjectType>(*this);
	}

private: // Internal methods

	void RemoveReference()
	{
		if(!IsValid()) return;
		
		Referencer->RemoveWeak();
		if(!Referencer->HasAnyReference())
			delete Referencer;
	}

private: // Fields

	ObjectType* Object;
	FSharedReferencer* Referencer;

private: // Friend class

	template<typename OtherT> friend class TSharedPtr;
};