
#include "RenderUtils.h"

namespace NRenderUtils
{
	using namespace NRenderConsts;
	
	class FPrivateCache
	{

	public: // Getters

		FORCEINLINE FVertexArrayId GetBoundArray() const { return BoundArrayId; }
		FORCEINLINE FBufferId GetBoundBuffer(EBufferTarget Target) const { auto search = BoundBufferIds.find(Target); return (search != BoundBufferIds.end()) ? search->second : Invalid::BufferId; }
		FORCEINLINE bool IsUnbindAllowed() const { return bAllowUnbind; }
		FORCEINLINE void AllowUnbind(bool Value) { bAllowUnbind = Value; }

	public: // Generating

		TArray<FVertexArrayId> GenerateArrays(const uint8 Num)
		{
			TArray<FVertexArrayId> result(Num);
			glGenVertexArrays(Num, result.data());

			for(const FVertexArrayId& arrayId : result)
				ArrayIds.push_back(arrayId);

			return result;
		}

		TArray<FBufferId> GenerateBuffers(const uint8 Num)
		{
			TArray<FBufferId> result(Num);
			glGenBuffers(Num, result.data());

			for(const FBufferId & bufferId : result)
				BufferIds.push_back(bufferId);

			return result;
		}

	public: // Binding / Unbinding

		void BindArray(FVertexArrayId Id)
		{
			if(BoundArrayId == Id) return;

			glBindVertexArray(Id);
			BoundArrayId = Id;
		}

		void UnbindArray(bool Force)
		{
			if(!Force && !bAllowUnbind) return;

			if(BoundArrayId == Invalid::VertexArrayId) return;

			glBindVertexArray(Invalid::VertexArrayId);
			BoundArrayId = Invalid::VertexArrayId;
		}

		void BindBuffer(EBufferTarget Target, FBufferId Id)
		{
			auto search = BoundBufferIds.find(Target);
			if(search != BoundBufferIds.end() && search->second == Id) return;

			glBindBuffer(Target, Id);

			if(search == BoundBufferIds.end())
				BoundBufferIds.insert({Target, Id});
			else
				search->second = Id;
		}

		void UnbindBuffer(EBufferTarget Target, bool Force)
		{
			if(!Force && !bAllowUnbind) return;

			auto search = BoundBufferIds.find(Target);
			if(search == BoundBufferIds.end()) return;

			glBindBuffer(Target, Invalid::BufferId);
			BoundBufferIds.erase(search);
		}

	public: // Delete

		void DeleteArrays(const TArray<FVertexArrayId>& Ids)
		{
			assert(Ids.size() > 0);

			for(FVertexArrayId id : Ids)
			{
				assert(id != Invalid::VertexArrayId);

				if(BoundArrayId == id)
				{
					glBindVertexArray(Invalid::VertexArrayId);
					BoundArrayId = Invalid::VertexArrayId;
				}

				for(auto it = ArrayIds.begin(); it != ArrayIds.end(); ++it)
				{
					if(*it == id)
					{
						ArrayIds.erase(it);
						break;
					}
				}
			}

			glDeleteVertexArrays((GLsizei)Ids.size(), Ids.data());
		}

		void DeleteBuffers(const TArray<FBufferId>& Ids)
		{
			assert(Ids.size() > 0);

			for(FBufferId id : Ids)
			{
				assert(id != Invalid::BufferId);

				for(auto it = BoundBufferIds.begin(); it != BoundBufferIds.end(); ++it)
				{
					if(it->second == id)
					{
						glBindBuffer(it->first, Invalid::BufferId);
						BoundBufferIds.erase(it);
						break;
					}
				}

				for(auto it = BufferIds.begin(); it != BufferIds.end(); ++it)
				{
					if(*it == id)
					{
						BufferIds.erase(it);
						break;
					}
				}
			}

			glDeleteBuffers((GLsizei)Ids.size(), Ids.data());
		}

	public: // Constructors

		FPrivateCache()
			: BoundArrayId(0)
			, bAllowUnbind(true)
		{}

		~FPrivateCache()
		{
			if(ArrayIds.size() > 0)
				glDeleteVertexArrays((GLsizei)ArrayIds.size(), ArrayIds.data());

			if(BufferIds.size() > 0)
				glDeleteBuffers((GLsizei)BufferIds.size(), BufferIds.data());
		}

	private: // Fields

		FVertexArrayId BoundArrayId;
		TFastMap<EBufferTarget, FBufferId> BoundBufferIds;

		TArray<FVertexArrayId> ArrayIds;
		TArray<FBufferId> BufferIds;

	private: // Primitive fields

		uint8 bAllowUnbind : 1;

	};

	static FPrivateCache* GCache = nullptr;

	void Init()
	{
		assert(!GCache);

		GCache = new FPrivateCache();
	}

	void Shutdown()
	{
		assert(GCache);

		delete GCache;
	}

	void AllowUnbind(bool Value)
	{
		GCache->AllowUnbind(Value);
	}

	bool IsUnbindAllowed()
	{
		return GCache->IsUnbindAllowed();
	}

	FVertexArrayId GenerateVertexArray()
	{
		return GCache->GenerateArrays(1)[0];
	}

	FBufferId GenerateBuffer()
	{
		return GCache->GenerateBuffers(1)[0];
	}

	TArray<FVertexArrayId> GenerateVertexArrays(const uint8 Num)
	{
		return GCache->GenerateArrays(Num);
	}

	TArray<FBufferId> GenerateBuffers(const uint8 Num)
	{
		return GCache->GenerateBuffers(Num);
	}

	void DeleteVertexArray(FBufferId Id)
	{
		GCache->DeleteArrays({Id});
	}

	void DeleteBuffer(FBufferId Id)
	{
		GCache->DeleteBuffers({Id});
	}

	void DeleteVertexArrays(const TArray<FVertexArrayId>& Ids)
	{
		GCache->DeleteArrays(Ids);
	}

	void DeleteBuffers(const TArray<FVertexArrayId>& Ids)
	{
		GCache->DeleteBuffers(Ids);
	}

	void BindVertexArray(FVertexArrayId Id)
	{
		GCache->BindArray(Id);
	}

	void BindBuffer(EBufferTarget Target, FBufferId Id)
	{
		GCache->BindBuffer(Target, Id);
	}

	FVertexArrayId GetBoundVertexArray()
	{
		return GCache->GetBoundArray();
	}

	FBufferId GetBoundBuffer(EBufferTarget Target)
	{
		return GCache->GetBoundBuffer(Target);
	}

	void UnbindVertexArray(bool Force)
	{
		GCache->UnbindArray(Force);
	}

	void UnbindBuffer(EBufferTarget Target, bool Force)
	{
		GCache->UnbindBuffer(Target, Force);
	}
}