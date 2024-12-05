extern "C" {
    void outstr_c(int line, char *str);
}


class X {

private:    
    int _loc;

public:

    X(int loc, const char *msg) {
        _loc = loc;
        outstr_c(loc, (char*)msg);
    }

    ~X() {
        outstr_c(_loc, "BYE");
    }

    void flux() {
        outstr_c(_loc, "FLUXXXXXX");
    }
};


    
X y(15, "fuuuuuuuuuuuuuuuuuuuuuu");

extern "C" {

    void execute_cpp(){
        X x(10, "hiiiiiiiiiiiiiiiiiiiiiiiiiiiii");
        x.flux();
    }

}