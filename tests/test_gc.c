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

// Циклы без root
void test_gc_collects_cycles(void) {
    LispVal* a = lisp_cons(NULL, NULL);
    LispVal* b = lisp_cons(NULL, NULL);

    a->data.cons.cdr = b;
    b->data.cons.cdr = a; // цикл

    gc_collect();

    // создаём новый объект → должен переиспользовать память
    LispVal* x = lisp_int(1);

    TEST_ASSERT_NOT_NULL(x);
}

//Циклы с root
void test_gc_preserves_cycles_with_root(void) {
    LispVal* a = lisp_cons(NULL, NULL);
    LispVal* b = lisp_cons(NULL, NULL);

    a->data.cons.cdr = b;
    b->data.cons.cdr = a;

    GC_PUSH(a);
    gc_collect();

    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_NOT_NULL(b);

    GC_POP();
}

//Переиспользование памяти
void test_gc_reuses_memory_slots(void) {
    LispVal* x = lisp_int(10);

    gc_collect(); // x умирает

    LispVal* y = lisp_int(20);

    TEST_ASSERT_TRUE(x == y); // тот же слот
}

//Смена root
void test_gc_root_update(void) {
    LispVal* x = lisp_int(1);
    GC_PUSH(x);

    gc_collect();
    TEST_ASSERT_EQUAL(1, x->data.int_val);

    // меняем root
    x = lisp_int(2);

    gc_collect();

    TEST_ASSERT_EQUAL(2, x->data.int_val);

    GC_POP();
}

//частично достежимый граф
void test_gc_partial_graph(void) {
    LispVal* alive = lisp_int(1);
    LispVal* dead = lisp_int(2);

    LispVal* root = lisp_cons(alive, NULL);

    GC_PUSH(root);
    gc_collect();

    // dead должен быть освобождён → переиспользуется
    LispVal* x = lisp_int(999);

    TEST_ASSERT_TRUE(x == dead);

    GC_POP();
}

//нагрузка на рекурсию mark
void test_gc_deep_tree(void) {
    LispVal* root = lisp_int(0);

    for (int i = 0; i < 50; i++) {
        root = lisp_cons(root, root);
    }

    GC_PUSH(root);
    gc_collect();

    TEST_ASSERT_NOT_NULL(root);

    GC_POP();
}

//освобождение строки
void test_gc_frees_strings(void) {
    LispVal* s = lisp_string("hello");

    gc_collect(); // должен освободить строку

    LispVal* x = lisp_string("world");

    TEST_ASSERT_NOT_NULL(x);
}

//стресс-тест
void test_gc_stress_allocation(void) {
    for (int i = 0; i < 1000; i++) {
        LispVal* x = lisp_int(i);
        (void)x;
    }

    TEST_ASSERT_TRUE(1); // главное — не упасть
}

//тест для thunk и closure
void test_gc_thunk_marking(void) {
    LispVal* arg = lisp_int(42);

    LispVal* thunk = lisp_thunk(NULL, arg, NULL);

    GC_PUSH(thunk);
    gc_collect();

    TEST_ASSERT_EQUAL(42, thunk->data.thunk.args->data.int_val);

    GC_POP();
}

//тест ошибки пользователя
void test_gc_push_pop_balance(void) {
    LispVal* x = lisp_int(1);

    GC_PUSH(x);
    GC_PUSH(x);

    gc_collect();

    TEST_ASSERT_EQUAL(1, x->data.int_val);

    GC_POP();
    GC_POP();
}