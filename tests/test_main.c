#include "unity.h"

void test_gc_preserves_root_object(void);
void test_gc_collects_unreachable_object(void);
void test_gc_marks_cons_graph(void);
void test_gc_collects_cycles(void);
void test_gc_partial_graph(void);
void test_gc_reuses_memory_slots(void);
void test_gc_closure_env_bug(void);
void test_gc_thunk_marking(void);
void test_gc_marks_deep_list(void);
void test_gc_collects_unreachable_list(void);
void test_gc_double_collect(void);
void test_gc_triggers_on_allocation(void);
void test_gc_preserves_cycles_with_root(void);
void test_gc_root_update(void);
void test_gc_deep_tree(void);
void test_gc_frees_strings(void);
void test_gc_stress_allocation(void);
void test_gc_push_pop_balance(void);

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_gc_preserves_root_object);
    RUN_TEST(test_gc_collects_unreachable_object);
    RUN_TEST(test_gc_marks_cons_graph);
    RUN_TEST(test_gc_marks_deep_list);
    RUN_TEST(test_gc_collects_unreachable_list);
    RUN_TEST(test_gc_double_collect);
    RUN_TEST(test_gc_triggers_on_allocation);
    RUN_TEST(test_gc_collects_cycles);
    RUN_TEST(test_gc_preserves_cycles_with_root);
    RUN_TEST(test_gc_reuses_memory_slots);
    RUN_TEST(test_gc_root_update);
    RUN_TEST(test_gc_partial_graph);
    RUN_TEST(test_gc_deep_tree);
    RUN_TEST(test_gc_frees_strings);
    RUN_TEST(test_gc_stress_allocation);
    RUN_TEST(test_gc_thunk_marking);
    RUN_TEST(test_gc_push_pop_balance);

    return UNITY_END();
}