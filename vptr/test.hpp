// ---------- 共同基类 ----------

struct A {
    int a1;
    int a2;

    virtual void f() {}
    virtual void g() {}
};

// ---------- 普通继承分支 ----------

struct B : A {
    int b1;
    int b2;

    void f() override {}
    virtual void h() {}
};

struct C : A {
    int c1;
    int c2;

    void g() override {}
    virtual void k() {}
};

// ---------- 多层单继承 ----------

struct D : B {
    int d1;
    int d2;

    void f() override {}
    void h() override {}
};

// ---------- 普通菱形继承 ----------

struct E : B, C {
    int e1;
    int e2;

    void f() override {}
    void g() override {}
};

// ---------- 虚继承分支 ----------

struct VB : virtual A {
    int vb1;
    int vb2;

    void f() override {}
    virtual void h() {}
};

struct VC : virtual A {
    int vc1;
    int vc2;

    void g() override {}
    virtual void k() {}
};

// ---------- 虚继承形成的菱形 ----------

struct VE : VB, VC {
    int ve1;
    int ve2;

    void f() override {}
    void h() override {}
};