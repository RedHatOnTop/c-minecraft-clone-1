#include "item/Inventory.hpp"
#include "item/Item.hpp"
#include "test_common.hpp"

void test_Inventory_AddItem_NullRegistrySafe() {
    Inventory inv(2);
    inv.setStack(0, ItemStack((ItemID)9999, 8, 0));

    ItemStack incoming((ItemID)9999, 1, 0);
    bool ok = inv.addItem(incoming);
    TEST_ASSERT(ok);
    TEST_ASSERT(incoming.count == 0);

    ItemStack secondSlot = inv.getStack(1);
    TEST_ASSERT(secondSlot.itemID == (ItemID)9999);
    TEST_ASSERT(secondSlot.count == 1);
}
