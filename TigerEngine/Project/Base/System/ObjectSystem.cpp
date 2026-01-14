#include "ObjectSystem.h"

void ObjectSystem::Destory(Handle h)
{
	if (h.index >= slots.size())
		return;

	Slot& slot = slots[h.index];
	if (slot.generation != h.generation) // ¼¼´ë°¡ ´Ù¸§ -> ÀÌ¹Ì handle °´Ã¼ ÆÄ±«µÊ
		return;

	slot.ptr->~Object();
	slot.ptr = nullptr;

	slot.generation++;

	freeSlots.push_back(h.index);
}