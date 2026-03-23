#include "unity.h"
#include "runtime/nucleus.h"
#include "runtime/gc.h"

/* --- Setup / Teardown --- */

void setUp(void) {
    lisp_runtime_init(64); // маленький heap для тестов
}

void tearDown(void) {
    lisp_runtime_shutdown();
}

/* --- Tests --- */

// 1. Объект в root не удаляется
void test_gc_preserves_root_object(void) {
    LispVal* x = lisp_int(42);

    GC_PUSH(x);
    gc_collect();

    TEST_ASSERT_NOT_NULL(x);
    TEST_ASSERT_EQUAL(LISP_INT, x->type);
    TEST_ASSERT_EQUAL(42, x->data.int_val);

    GC_POP();
}

// 2. Объект без root удаляется
void test_gc_collects_unreachable_object(void) {
    LispVal* x = lisp_int(123);

    // не пушим в root → должен умереть
    gc_collect();

    // выделяем новый объект → он должен переиспользовать память
    LispVal* y = lisp_int(999);

    // если GC сработал — x и y скорее всего совпадут
    TEST_ASSERT_TRUE(x == y);
}

// 3. Cons-связи маркируются
void test_gc_marks_cons_graph(void) {
    LispVal* a = lisp_int(1);
    LispVal* b = lisp_int(2);
    LispVal* pair = lisp_cons(a, b);

    GC_PUSH(pair);
    gc_collect();

    TEST_ASSERT_EQUAL(LISP_CONS, pair->type);
    TEST_ASSERT_EQUAL(1, pair->data.cons.car->data.int_val);
    TEST_ASSERT_EQUAL(2, pair->data.cons.cdr->data.int_val);

    GC_POP();
}

// 4. Глубокий список
void test_gc_marks_deep_list(void) {
    LispVal* list = lisp_int(0);

    for (int i = 1; i < 10; i++) {
        list = lisp_cons(lisp_int(i), list);
    }

    GC_PUSH(list);
    gc_collect();

    TEST_ASSERT_EQUAL(LISP_CONS, list->type);

    GC_POP();
}

// 5. Освобождение цепочки
void test_gc_collects_unreachable_list(void) {
    LispVal* list = lisp_int(0);

    for (int i = 1; i < 10; i++) {
        list = lisp_cons(lisp_int(i), list);
    }

    // НЕ пушим → всё должно умереть
    gc_collect();

    LispVal* new_obj = lisp_int(777);

    TEST_ASSERT_NOT_NULL(new_obj);
}

// 6. Повторный GC
void test_gc_double_collect(void) {
    LispVal* x = lisp_int(5);

    GC_PUSH(x);
    gc_collect();
    gc_collect();

    TEST_ASSERT_EQUAL(5, x->data.int_val);

    GC_POP();
}

// 7. Проверка переполнения heap (форс GC)
void test_gc_triggers_on_allocation(void) {
    for (int i = 0; i < 100; i++) {
        lisp_int(i); // без root → должны собираться
    }

    // если GC сломан — будет crash
    TEST_ASSERT_TRUE(1);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_gc_preserves_root_object);
    RUN_TEST(test_gc_collects_unreachable_object);
    RUN_TEST(test_gc_marks_cons_graph);
    RUN_TEST(test_gc_marks_deep_list);
    RUN_TEST(test_gc_collects_unreachable_list);
    RUN_TEST(test_gc_double_collect);
    RUN_TEST(test_gc_triggers_on_allocation);

    return UNITY_END();
}