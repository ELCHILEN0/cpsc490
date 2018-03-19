#include <memory>
#include <iostream>
#include <vector>

/**
 * Each section of this file contains some code and a specification.
 *
 * It compiles.
 *
 * Is it safe (memory leaks, does it maintain its own state properly)? Does it follow the spec? You decide.
 *
 * If (when) you find an error, fix it and explain why you did.
 *
 * Warning: some comments a slightly "misguided" - test anything suspicious, sometimes
 * a comment will be wrong in order to explain what the hypothetical author thought would
 * happen. If so, alter the code to match the closest working interpretation of that comment.
 *
 * To avoid this devolving into overly hardcore mindgame, have a hint: there should be
 * around 3 errors.
 *
 * The road to hell is paved with good intentions...
 */

// --- INTERFACES

/**
 * This is part of a hypothetical graphics library.
 */

/**
 * This is the abstract interface for drawing things - to avoid the complexity inherent
 * to real graphics applications it behaves in a very "text-like" manner.
 *
 * Note: don't worry if the output looks like junk (the stack and set don't nest properly
 * for example), that's not really the point here. Point is: does it do what it says it does?
 */
class DrawContext{
public:
    virtual void write(char c) = 0;
    virtual void newLine() = 0;
};

/**
 * It is an interface class for widgets - widgets should inherit from it
 * and implement its methods.
 */
class Drawable{
public:
    virtual ~Drawable() = default;

    // draw the drawable using the given context
    virtual void draw(DrawContext&) = 0;

    // default overrideable implementation of logging init
    // this will only be called on construction
    virtual void logSetup(){
        std::cerr << "init logging base Drawable\n";
    }
};

// --- IMPLEMENTATIONS

/**
 * A simple implementation of the DrawContext, backed by a std::string.
 * Good for testing.
 */
class StringDrawContext: public DrawContext{
public:
    // default initialisation
    StringDrawContext(){}

    // start with a given string, for use in testing fixtures
    StringDrawContext(std::string s):
        result{s}
    {}

    // helper for writing a whole string at once. whose idea was that
    // interface anyway? so inconvenient...
    void write(std::string s){
        result += s;
    }
    void write(char c){
        result += c;
    }
    void newLine(){
        write("\n");
    }

    std::string const& getResult(){
        return result;
    }
private:
    std::string result;
};

/**
 * For some reason management decreed all elements (read: any character, including whitespace and
 * newlines) of text UIs had to be preceded by the string "fairy". The devs opted to "refine" the
 * existing StringDrawContext without polluting it with this madness.
 */
class FairyStringDrawContext: public StringDrawContext{
public:
    // should inherit the default constructor

    // initialise 
    FairyStringDrawContext(std::string s) : StringDrawContext(s)
    { }

    // implement madness
    void write(char c){
        StringDrawContext::write("fairy");
        StringDrawContext::write(c);
    }
    // no need to override newLine, as it delegates to write() anywayå
};


class WidgetStack: public Drawable{
private:
    std::vector<std::unique_ptr<Drawable>> widgets;
public:
    WidgetStack(std::vector<std::unique_ptr<Drawable>> widgets):
        widgets{std::move(widgets)}
    {
        logSetup();
    }

    // draw the widgets stacked vertically by separating them with newlines
    void draw(DrawContext& ctx){
        for(auto& widget: widgets){
            widget->draw(ctx);
            ctx.newLine();
        }
    }

    void logSetup(){
        std::cerr << "setup WidgetStack\n";
    }
};

class WidgetSet: public Drawable{
private:
    std::vector<std::unique_ptr<Drawable>> widgets;
public:
    WidgetSet(std::vector<std::unique_ptr<Drawable>> widgets):
        widgets{std::move(widgets)}
    {
        logSetup();
    }

    // draw the widgets separated horizontally with spaces
    void draw(DrawContext& ctx){
        for(auto& widget: widgets){
            widget->draw(ctx);
            ctx.write(' ');
        }
    }

    void logSetup(){
        std::cerr << "setup WidgetSet\n";
    }
};

// represents a single character
class CharWidget: public Drawable{
public:
    CharWidget(char ch): ch{ch} {
        logSetup();
    }

    void draw(DrawContext& ctx){
        ctx.write(ch);
    }

    void logSetup(){
        std::cerr << "setup CharWidget\n";
    }

    // ~CharWidget() {
    //     std::cerr << "~CW" << std::endl;
    // }
private:
    char ch;
};

// --- EXAMPLE APP

int main(){
    StringDrawContext s1;
    FairyStringDrawContext s2("");

    std::vector<std::unique_ptr<Drawable>> widgets1;
    widgets1.push_back(std::make_unique<CharWidget>('a'));
    widgets1.push_back(std::make_unique<CharWidget>('b'));
    widgets1.push_back(std::make_unique<CharWidget>('c'));

    std::vector<std::unique_ptr<Drawable>> widgets2;
    widgets2.push_back(std::make_unique<CharWidget>('d'));
    widgets2.push_back(std::make_unique<CharWidget>('e'));
    widgets2.push_back(std::make_unique<CharWidget>('f'));

    std::vector<std::unique_ptr<Drawable>> widgets3;
    widgets3.push_back(std::make_unique<WidgetSet>(std::move(widgets1)));
    widgets3.push_back(std::make_unique<WidgetSet>(std::move(widgets2)));
    
    WidgetSet to_draw(std::move(widgets3));

    to_draw.draw(s1);
    to_draw.draw(s2);

    std::cout << s1.getResult() << "\n\n" << s2.getResult() << "\n";

    return 0;
}

