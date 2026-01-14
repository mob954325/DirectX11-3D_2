#include "ObjectSystem.h"

void ObjectSystem::Destory(Handle h)
{
	if (h.index >= slots.size())
		return;

	Slot& slot = slots[h.index];
	if (slot.generation != h.generation) // ¼¼´ë°¡ ´Ù¸§ -> ÀÌ¹Ì handle °´Ã¼ ÆÄ±«µÊ
		return;

	delete slot.ptr;
	slot.ptr = nullptr;

	slot.ptr->~Object();
	slot.generation++;

	freeSlots.push_back(h.index);
}