#include <string>
#include <list>
#include <memory>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <chrono>
#include <type_traits>
#include <algorithm>
#include <random>
#include <fstream>

/**
 * \namespace triglav 
 * This is the main (and only) namespace used in the Triglav implementation.
 */
namespace triglav
{
class type_expression;

// Some template booleans used by the concepts.
template <typename SOME_CLASS> constexpr const bool is_reportable{false};
template <typename SOME_CLASS> constexpr const bool is_an_iterator{false};
template <typename SOME_CLASS> constexpr const bool has_exactly_one_inner_class{false};
template <typename SOME_CLASS> constexpr const bool has_a_list_of_inner_class_instances{false};
template <typename SOME_CLASS> constexpr const bool has_a_vector_of_inner_class_instances{false};
template <typename SOME_CLASS> constexpr const bool has_a_name{false};
template <typename SOME_CLASS> constexpr const bool can_be_expandable{false};
template <typename SOME_CLASS> constexpr const bool has_a_mode{false};
template <typename SOME_CLASS> constexpr const bool has_internal_report{false};

/**
 * \class generic_name_item 
 * It is used as a base class for the generic_name_item_token and generic_name_item_placeholder,
 */
class generic_name_item
{
public:
    virtual ~generic_name_item() {}
    virtual void report(std::ostream & s) const = 0;
    virtual bool get_is_expandable() const = 0;

    virtual const std::string & get_placeholder_name() const = 0;
    virtual std::shared_ptr<type_expression>& get_type_expression() = 0;
};

template <> constexpr const bool is_reportable<generic_name_item> {true};
template <> constexpr const bool can_be_expandable<generic_name_item> {true};


/**
 * \class generic_name_item_token
 * It represents a string literal within a generic name.
 */
class generic_name_item_token: public generic_name_item
{
private:
    static const std::string dummy_placeholder_name;
    static std::shared_ptr<type_expression> dummy_type_expression_pointer;

    const std::string name;

public:
    generic_name_item_token(const std::string & n): name{n} {}
    virtual void report(std::ostream & s) const override;

    virtual bool get_is_expandable() const override {
        return false;
    }

    virtual const std::string & get_placeholder_name() const override {
        return dummy_placeholder_name;    // not really used
    }

    virtual std::shared_ptr<type_expression>& get_type_expression() override {
        return dummy_type_expression_pointer;    // not really used
    }

    const std::string & get_name() const {
        return name;
    }
};

template <> constexpr const bool is_reportable<generic_name_item_token> {true};
template <> constexpr const bool has_a_name<generic_name_item_token> {true};
template <> constexpr const bool can_be_expandable<generic_name_item_token> {true};

/**
 * \class generic_name_item_placeholder
 * It represents a placeholder within a generic name. Its corresponding syntax is for example \f$(X:boolean)\f$ with "boolean" being a type name
 * and "X" the placeholder name.
 */
class generic_name_item_placeholder: public generic_name_item
{
private:
    const std::string placeholder_name;
    std::shared_ptr<type_expression> my_type_expression;

public:
    generic_name_item_placeholder(const std::string & n, std::shared_ptr<type_expression> && e): placeholder_name{n}, my_type_expression{std::move(e)} {}

    virtual void report(std::ostream & s) const override;

    virtual bool get_is_expandable() const override {
        return true;
    }

    virtual const std::string & get_placeholder_name() const override {
        return placeholder_name;
    }

    virtual std::shared_ptr<type_expression>& get_type_expression() override {
        return my_type_expression;
    }
};

template <> constexpr const bool is_reportable<generic_name_item_placeholder> {true};
template <> constexpr const bool can_be_expandable<generic_name_item_placeholder> {true};


class my_iterator;

/**
 * \class generic_name
 * This is an abstract class representing a name of a recursive enumeration value. It has only one implementation, the generic_name_list.
 */
class generic_name
{
public:
    virtual void report(std::ostream & s) const = 0;
    virtual ~generic_name() {}

    virtual bool get_contains_expandable_items() const = 0;

    virtual std::list<std::shared_ptr<generic_name_item>> & get_list_of_name_items() = 0;

    virtual void update_iterator(my_iterator & target) = 0;

    virtual std::string get_actual_name(my_iterator & source) = 0;
};

/**
 * \class generic_name_list
 * This is the only implementation of the generic_name. It is a list of generic_name_item objects.
 */
class generic_name_list: public generic_name
{
protected:
    std::list<std::shared_ptr<generic_name_item>> list_of_name_items;
public:
    using inner_class = generic_name_item;

    virtual ~generic_name_list() {}

    void add(std::shared_ptr<generic_name_item> && i)
    {
        list_of_name_items.push_back(std::move(i));
    }

    virtual void report(std::ostream & s) const override;

    virtual bool get_contains_expandable_items() const override;

    virtual std::list<std::shared_ptr<generic_name_item>> & get_list_of_name_items() override {
        return list_of_name_items;
    }

    const std::list<std::shared_ptr<generic_name_item>> & get_list() const {
        return list_of_name_items;
    }

    std::list<std::shared_ptr<generic_name_item>> & get_list() {
        return list_of_name_items;
    }

    virtual void update_iterator(my_iterator & target) override;

    virtual std::string get_actual_name(my_iterator & source) override;
};

template <> constexpr const bool has_exactly_one_inner_class<generic_name_list> {true};
template <> constexpr const bool is_reportable<generic_name_list> {true};
template <> constexpr const bool has_a_list_of_inner_class_instances<const generic_name_list> {true};
template <> constexpr const bool has_a_list_of_inner_class_instances<generic_name_list> {true};

// Here are some concepts describing the classes.
/**
 * \concept AClassPosessingExactlyOneInnerClass
 * Such classes have usually a list or a vector of shared pointer to the inner class instances.
 * They should have the type inner_class defined and public.
 */
template <typename SOME_CLASS> concept AClassPosessingExactlyOneInnerClass = has_exactly_one_inner_class<SOME_CLASS>;

/**
 * \concept AReportableClass
 * Such classes have a method report.
 */
template <typename SOME_CLASS> concept AReportableClass = is_reportable<SOME_CLASS>;

/**
 * \concept AClassWithAListOfInnerClassInstances
 * Such classes have a list of inner class instances. This concept is used for iterations, for example iterate_with_first.
 */
template <typename SOME_CLASS> concept AClassWithAListOfInnerClassInstances = has_a_list_of_inner_class_instances<SOME_CLASS>;

/**
 * \concept AClassWithAVectorOfInnerClassInstances
 * Such classes have a vector of inner class instances. This concept is used for iterations, for example iterate_with_first.
 */
template <typename SOME_CLASS> concept AClassWithAVectorOfInnerClassInstances = has_a_vector_of_inner_class_instances<SOME_CLASS>;

/**
 * \concept AClassWithAName
 * Such classes have a name. They also have a get_name getter.
 */
template <typename SOME_CLASS> concept AClassWithAName = has_a_name<SOME_CLASS>;

/**
 * \concept AnExpandableClass
 * Such classes have a method get_is_expandable to check whether their instances are expandable in runtime.
 */
template <typename SOME_CLASS> concept AnExpandableClass = can_be_expandable<SOME_CLASS>;

/**
 * \concept AClassWithAMode
 * Such classes have a method get_mode returning the enumeration variable_mode.
 */
template <typename SOME_CLASS> concept AClassWithAMode = has_a_mode<SOME_CLASS>;

/**
 * \concept AClassWithACollectionOfInnerClassInstances
 * Such classes have either a list or a vector of inner class instances.
 */
template <typename SOME_CLASS> concept AClassWithACollectionOfInnerClassInstances = AClassWithAListOfInnerClassInstances<SOME_CLASS> || AClassWithAVectorOfInnerClassInstances<SOME_CLASS>;

/**
 * \concept AReportableClassWithInnerReport
 * Such classes have a method inner_report. 
 */
template <typename SOME_CLASS> concept AReportableClassWithInnerReport = AReportableClass<SOME_CLASS> && has_internal_report<SOME_CLASS>;

/**
 * \concept AReportableClassWithInnerReportAndName
 * Such classes have both a name and an inner_report method.
 */
template <typename SOME_CLASS> concept AReportableClassWithInnerReportAndName = AReportableClassWithInnerReport<SOME_CLASS> && AClassWithAName<SOME_CLASS>;


/**
 * report can be applied to any reportable class.
 */
template <AReportableClass SOME_CLASS>
void report(const SOME_CLASS & x, std::ostream & s)
{
    x.report(s);
}

/**
 * This is a report function printing out a type name and then its internal report.
 */
template <AReportableClassWithInnerReport SOME_CLASS>
void report_with_internal(const SOME_CLASS & x, std::ostream & s, auto type_name)
{
    s << type_name << " ";
    x.internal_report(s);
}

/**
 * A special report function printing out a type name, get_name result
 * and then internal_report.
 */
template <AReportableClassWithInnerReportAndName SOME_CLASS>
void report_with_internal(const SOME_CLASS & x, std::ostream & s, auto type_name)
{
    s << type_name << " " << x.get_name() << ":";
    x.internal_report(s);
}

/**
 * Adding an instance of an inner class to a class instance that contains exactly one inner class.
 */
template <AClassPosessingExactlyOneInnerClass SOME_CLASS>
void add(SOME_CLASS & x, typename SOME_CLASS::inner_class && y)
{
    x.add(std::move(y));
}

/**
 * A covenience version of get_name.
 */
template <AClassWithAName SOME_CLASS>
const std::string & get_name(const SOME_CLASS & x)
{
    return x.get_name();
}

/**
 * A wrapper for a get_is_expandable getter.
 */
template <AnExpandableClass SOME_CLASS>
bool get_is_expandable(const SOME_CLASS & x)
{
    return x.get_is_expandable();
}

template <AClassWithAListOfInnerClassInstances SOME_CLASS>
const std::list<std::shared_ptr<typename SOME_CLASS::inner_class>>& get_list(const SOME_CLASS & x)
{
    return x.get_list();
}

template <AClassWithAListOfInnerClassInstances SOME_CLASS>
std::list<std::shared_ptr<typename SOME_CLASS::inner_class>>& get_list(SOME_CLASS & x)
{
    return x.get_list();
}

template <AClassWithAVectorOfInnerClassInstances SOME_CLASS>
const std::vector<std::shared_ptr<typename SOME_CLASS::inner_class>>& get_vector(const SOME_CLASS & x)
{
    return x.get_vector();
}

template <AClassWithAVectorOfInnerClassInstances SOME_CLASS>
std::vector<std::shared_ptr<typename SOME_CLASS::inner_class>>& get_vector(SOME_CLASS & x)
{
    return x.get_vector();
}

template <AClassWithAVectorOfInnerClassInstances SOME_CLASS>
void iterate(SOME_CLASS & x, auto lambda)
{
    std::for_each(get_vector(x).begin(), get_vector(x).end(), lambda);
}

template <AClassWithAListOfInnerClassInstances SOME_CLASS>
void iterate(SOME_CLASS & x, auto lambda)
{
    std::for_each(get_list(x).begin(), get_list(x).end(), lambda);
}

template <AClassWithAListOfInnerClassInstances SOME_CLASS>
void iterate_with_first(SOME_CLASS & x, std::ostream & s, auto lambda)
{
    bool first=true;
    iterate(x,[&first,&s,&lambda](auto i) {
        if (!first) s << ",";
        lambda(i);
        first=false;
    });
}

template <AClassWithAVectorOfInnerClassInstances SOME_CLASS>
void iterate_with_first(SOME_CLASS & x, std::ostream & s, auto lambda)
{
    bool first=true;
    iterate(x,[&first,&s,&lambda](auto i) {
        if (!first) s << ",";
        lambda(i);
        first=false;
    });
}

/**
 * \class type_expression
 * It represents a type of a placeholder or a variable.
 */
class type_expression
{
public:
    virtual ~type_expression() {}
    virtual void report(std::ostream & s) const = 0;

    virtual bool get_is_type_name() const = 0;
    virtual const std::string & get_type_name() const = 0;
};

template <> constexpr const bool is_reportable<type_expression> {true};

/**
 * \class type_expression_type_name
 * This class is commonly used to denote a type of a variable or a placeholder.
 */
class type_expression_type_name: public type_expression
{
private:
    const std::string name;
public:
    type_expression_type_name(const std::string & n): name{n} {}
    virtual void report(std::ostream & s) const override;
    virtual bool get_is_type_name() const override {
        return true;
    }
    virtual const std::string & get_type_name() const override {
        return name;
    }
};

template <> constexpr const bool is_reportable<type_expression_type_name> {true};

/**
 * \class type
 * This class has a list of generic_name objects that can depend on various types and can be expanded in runtime using the "expand" command.
 */
class type
{
private:
    const std::string name;

    std::list<std::shared_ptr<generic_name>> list_of_generic_names;

public:
    using inner_class=generic_name;

    type(const std::string & n): name{n} {}
    void add(std::shared_ptr<generic_name> && n)
    {
        list_of_generic_names.push_back(std::move(n));
    }

    void report(std::ostream & s) const;

    const std::string & get_name() const {
        return name;
    }

    std::list<std::shared_ptr<generic_name>> & get_list_of_generic_names() {
        return list_of_generic_names;
    }

    std::list<std::shared_ptr<generic_name>> & get_list() {
        return list_of_generic_names;
    }
    const std::list<std::shared_ptr<generic_name>> & get_list() const {
        return list_of_generic_names;
    }
};

template <> constexpr const bool has_exactly_one_inner_class<type> {true};
template <> constexpr const bool has_a_list_of_inner_class_instances<type> {true};
template <> constexpr const bool has_a_list_of_inner_class_instances<const type> {true};
template <> constexpr const bool is_reportable<type> {true};

/**
 * \enum variable_mode
 * This enum is used to represent the information whether a variable (or a variable instance) is input, hidden or output.
 */
enum class variable_mode
{
    INPUT,
    OUTPUT,
    HIDDEN
};

/**
 * \class variable
 * This is a base class for all variable classes. Apart from the type_expression representing the variable's type it has a list
 * of usual values.
 */
class variable
{
private:
    generic_name_list my_name;
    std::shared_ptr<type_expression> my_type;

    std::list<std::string> list_of_usual_values;
    bool has_usual_values;

public:
    variable(std::shared_ptr<type_expression> && t): my_type{std::move(t)}, has_usual_values{false} {}

    void internal_report(std::ostream & s) const;

    void add_name_item(std::shared_ptr<generic_name_item> && i)
    {
        my_name.add(std::move(i));
    }

    virtual ~variable() {}

    virtual void report(std::ostream & s) const = 0;

    virtual variable_mode get_mode() const = 0;

    generic_name& get_name_list() {
        return my_name;
    }

    std::shared_ptr<type_expression>& get_type() {
        return my_type;
    }

    void add_usual_value(std::string & v) {
        has_usual_values = true;
        list_of_usual_values.push_back(v);
    }

    bool get_has_usual_values() const {
        return has_usual_values;
    }

    const std::list<std::string> & get_list_of_usual_values() const {
        return list_of_usual_values;
    }
};

template <> constexpr const bool is_reportable<variable> {true};
template <> constexpr const bool has_a_mode<variable> {true};

/**
 * \class input_variable
 * This class represents a family of input variable instances. In the documentation they are associated with the function \f$f:I \cup H \to V\f$ representing
 * the environment state. The set \f$I\f$ is a set of all input variable instances.
 */
class input_variable: public variable
{
public:
    input_variable(std::shared_ptr<type_expression> && t): variable{std::move(t)} {}
    virtual void report(std::ostream & s) const override;
    virtual variable_mode get_mode() const override {
        return variable_mode::INPUT;
    }
};

template <> constexpr const bool is_reportable<input_variable> {true};
template <> constexpr const bool has_a_mode<input_variable> {true};
template <> constexpr const bool has_internal_report<input_variable> {true};

/**
 * \class output_variable
 * This class represents a family of output variable instances. The set of all output variable instances is denoted as \f$O\f$. 
 * An action is any function \f$a: O \to V\f$.
 */
class output_variable: public variable
{
public:
    output_variable(std::shared_ptr<type_expression> && t): variable{std::move(t)} {}
    virtual void report(std::ostream & s) const override;
    virtual variable_mode get_mode() const override {
        return variable_mode::OUTPUT;
    }
};

template <> constexpr const bool is_reportable<output_variable> {true};
template <> constexpr const bool has_a_mode<output_variable> {true};
template <> constexpr const bool has_internal_report<output_variable> {true};

/**
 * \class hidden_variable
 * This class represents a family of hidden variable instances. In the documentation they are associated with the function \f$f:I \cup H \to V\f$ representing
 * the environment state. The set \f$H\f$ is a set of all hidden variable instances.
 */
class hidden_variable: public variable
{
public:
    hidden_variable(std::shared_ptr<type_expression> && t): variable{std::move(t)} {}
    virtual void report(std::ostream & s) const override;
    virtual variable_mode get_mode() const override {
        return variable_mode::HIDDEN;
    }
};

template <> constexpr const bool is_reportable<hidden_variable> {true};
template <> constexpr const bool has_a_mode<hidden_variable> {true};
template <> constexpr const bool has_internal_report<hidden_variable> {true};

/**
 * \class value_instance
 * This class represents an actual instance of a value within a type. The set of all value instances is denoted as \f$V\f$.
 */
class value_instance
{
private:
    unsigned level;
    const std::string name;

public:
    value_instance(const std::string & n, unsigned l): name{n}, level{l} {}

    const std::string & get_name() const {
        return name;
    }

    unsigned get_level() const {
        return level;
    }
};

template <> constexpr const bool has_a_name<value_instance> {true};


class type_instance;

/**
 * \class variable_instance
 * This class represents an actual instance of a variable.
 */
class variable_instance
{
protected:
    const std::string name;
    std::shared_ptr<variable> my_variable;
    std::shared_ptr<type_instance> my_type_instance;

    std::vector<std::shared_ptr<value_instance>> vector_of_value_instances;

    bool has_usual_values;

public:
    using is_an_instance_of=variable;
    using inner_class=value_instance;

    void internal_report(std::ostream & s) const;

    variable_instance(const std::string & n, std::shared_ptr<variable> && h): name{n}, my_variable{std::move(h)}, has_usual_values{my_variable->get_has_usual_values()} {}

    virtual void report(std::ostream & s) const = 0;

    void add(std::shared_ptr<value_instance> && i) {
        vector_of_value_instances.push_back(std::move(i));
    }
    
    void set_type_instance(std::shared_ptr<type_instance> & t)
    {
        my_type_instance = t;
    }
    
    std::shared_ptr<type_instance>& get_type_instance()
    {
        return my_type_instance;
    }

    virtual variable_mode get_mode() const = 0;

    const std::string & get_name() const {
        return name;
    }

    std::vector<std::shared_ptr<value_instance>>& get_vector_of_value_instances() {
        return vector_of_value_instances;
    }

    const std::vector<std::shared_ptr<value_instance>>& get_vector_of_value_instances() const {
        return vector_of_value_instances;
    }

    std::vector<std::shared_ptr<value_instance>>& get_vector() {
        return vector_of_value_instances;
    }
    const std::vector<std::shared_ptr<value_instance>>& get_vector() const {
        return vector_of_value_instances;
    }
    
    bool get_has_usual_values() const { return has_usual_values; }
    
    const std::list<std::string>& get_list_of_usual_values() const { return my_variable->get_list_of_usual_values(); }
};

template <> constexpr const bool has_a_name<variable_instance> {true};
template <> constexpr const bool has_a_mode<variable_instance> {true};
template <> constexpr const bool is_reportable<variable_instance> {true};
template <> constexpr const bool has_a_vector_of_inner_class_instances<variable_instance> {true};
template <> constexpr const bool has_a_vector_of_inner_class_instances<const variable_instance> {true};


class input_variable_instance: public variable_instance
{
public:
    input_variable_instance(const std::string & n, std::shared_ptr<variable> && h): variable_instance{n, std::move(h)} {}

    virtual void report(std::ostream & s) const override;

    virtual variable_mode get_mode() const override {
        return variable_mode::INPUT;
    }
};

template <> constexpr const bool has_a_name<input_variable_instance> {true};
template <> constexpr const bool has_a_mode<input_variable_instance> {true};
template <> constexpr const bool is_reportable<input_variable_instance> {true};
template <> constexpr const bool has_internal_report<input_variable_instance> {true};

class output_variable_instance: public variable_instance
{
public:
    output_variable_instance(const std::string & n, std::shared_ptr<variable> && h): variable_instance{n, std::move(h)} {}
    virtual void report(std::ostream & s) const override;
    virtual variable_mode get_mode() const override {
        return variable_mode::OUTPUT;
    }
};

template <> constexpr const bool has_a_name<output_variable_instance> {true};
template <> constexpr const bool has_a_mode<output_variable_instance> {true};
template <> constexpr const bool is_reportable<output_variable_instance> {true};
template <> constexpr const bool has_internal_report<output_variable_instance> {true};


class hidden_variable_instance: public variable_instance
{
public:
    hidden_variable_instance(const std::string & n, std::shared_ptr<variable> && h): variable_instance{n, std::move(h)} {}
    virtual void report(std::ostream & s) const override;
    virtual variable_mode get_mode() const override {
        return variable_mode::HIDDEN;
    }
};

template <> constexpr const bool has_a_name<hidden_variable_instance> {true};
template <> constexpr const bool has_a_mode<hidden_variable_instance> {true};
template <> constexpr const bool is_reportable<hidden_variable_instance> {true};
template <> constexpr const bool has_internal_report<hidden_variable_instance> {true};

class agent;


class set_of_possible_values: public std::vector<std::string>
{
public:
    void report(std::ostream & s) const;

    bool get_contains(const std::string & s)
    {
        return std::find_if(begin(),end(),[&s](auto i) {
            return i==s;
        })!=end();
    }
};

/**
 * \class belief
 * Unlike in svarog we do not use belief that attempts to cover all the states.
 * Instead we use projections - for each hidden variables there is a separate projection
 * containing as many states as many values it has. This allows to handle many more hidden variables.
 */
class belief
{
public:

    class projection
    {
    private:
        variable_instance & my_hidden_variable_instance;

        class state
        {
        private:
            bool possible;
            float probability;
            value_instance & my_value_instance;
        public:
            state(value_instance & i): possible{true}, my_value_instance{i}, probability{1.0f} {}
            state(const state & s): possible{s.possible}, my_value_instance{s.my_value_instance}, probability{s.probability} {}

            value_instance & get_value_instance() {
                return my_value_instance;
            }

            bool get_is_possible() const {
                return possible;
            }

            void set_is_impossible() {
                possible = false;    // the projection should be normalized afterwards
                probability = 0.0f;
            }

            void set_is_possible() {
                possible = true;    // the projection should be normalized afterwards
                probability = 1.0f;
            }

            void set_probability(float p) {
                probability = p;
            }

            float get_probability() const {
                return probability;
            }

            void aggregate(const state & s)
            {
                possible = possible || s.get_is_possible();
                probability += s.get_probability();     // after aggregation we must normalize the belief
            }
        };


        std::list<std::shared_ptr<state>> list_of_states;

    public:
        using inner_class = state;

        projection(variable_instance & i);

        projection(const projection & p);

        void report(std::ostream & s) const;

        bool get_all_states_are_possible() const;

        unsigned get_amount_of_possible_states() const;

        variable_instance & get_variable_instance() {
            return my_hidden_variable_instance;
        }

        void all_but_one_state_is_impossible(const std::string & v);

        void this_state_is_impossible(const std::string & v);

        std::string get_value() const;

        void normalize();

        bool get_is_not_certain() const;

        std::list<std::shared_ptr<state>>& get_list_of_states() {
            return list_of_states;
        }

        const std::list<std::shared_ptr<state>>& get_list_of_states() const {
            return list_of_states;
        }

        std::list<std::shared_ptr<state>>& get_list() {
            return list_of_states;
        }

        const std::list<std::shared_ptr<state>>& get_list() const {
            return list_of_states;
        }

        void aggregate(const projection & p);

        void initialize_for_aggregation();

        void update_set_of_possible_values(set_of_possible_values & target);

        void set_possible_values(set_of_possible_values & st);
    };


    std::vector<std::shared_ptr<projection>> vector_of_projections;
public:
    using inner_class = projection;

    belief(agent & a);

    belief(const belief & b);

    void report(std::ostream & s) const;

    /**
     * It returns true if and only if there is only one possible state in the given projection.
     */
    bool get_is_known(variable_instance & i) const;

    void update_set_of_possible_values(set_of_possible_values & target, variable_instance & i);

    void set_possible_values(set_of_possible_values & st, variable_instance & i);

    std::string get_value(variable_instance & i) const;

    void all_but_one_state_is_impossible(variable_instance & i, const std::string & v);

    void this_state_is_impossible(variable_instance & i, const std::string & v);

    void normalize();

    void initialize_for_aggregation();

    void aggregate(const belief & b);

    std::vector<std::shared_ptr<projection>>& get_vector_of_projections() {
        return vector_of_projections;
    }

    const std::vector<std::shared_ptr<projection>>& get_vector_of_projections() const {
        return vector_of_projections;
    }

    std::vector<std::shared_ptr<projection>>& get_vector() {
        return vector_of_projections;
    }
    const std::vector<std::shared_ptr<projection>>& get_vector() const {
        return vector_of_projections;
    }
};

template <> constexpr const bool is_reportable<belief> {true};
template <> constexpr const bool has_a_vector_of_inner_class_instances<belief> {true};
template <> constexpr const bool has_a_vector_of_inner_class_instances<const belief> {true};

template <> constexpr const bool is_reportable<belief::projection> {true};
template <> constexpr const bool has_a_list_of_inner_class_instances<belief::projection> {true};
template <> constexpr const bool has_a_list_of_inner_class_instances<const belief::projection> {true};

/**
 * A type_instance is created in runtime by the expand command and is based on type.
 */
class type_instance
{
private:
    const std::string name;
    
    std::shared_ptr<type> my_type;

    std::list<std::shared_ptr<value_instance>> list_of_value_instances;

public:
    using is_an_instance_of = type;
    using inner_class = value_instance;

    type_instance(const std::string & n, std::shared_ptr<type> && t): name{n}, my_type{std::move(t)} {}

    void report(std::ostream & s) const;

    const std::string & get_name() const {
        return name;
    }

    std::weak_ptr<type> get_type() {
        return my_type;
    }

    void add(std::shared_ptr<value_instance> && i);

    std::list<std::shared_ptr<value_instance>> & get_list_of_value_instances() {
        return list_of_value_instances;
    }

    std::list<std::shared_ptr<value_instance>> & get_list() {
        return list_of_value_instances;
    }
    const std::list<std::shared_ptr<value_instance>> & get_list() const {
        return list_of_value_instances;
    }
};

template <> constexpr const bool is_reportable<type_instance> {true};
template <> constexpr const bool has_exactly_one_inner_class<type_instance> {true};
template <> constexpr const bool has_a_list_of_inner_class_instances<type_instance> {true};
template <> constexpr const bool has_a_list_of_inner_class_instances<const type_instance> {true};

/**
 * \class my_iterator
 * This is a common base class for iterators that generate cartesian product of various sets.
 * They are used to expand variables or types.
 */
class my_iterator
{
protected:
    agent & my_agent;

    std::map<std::string, std::list<std::string>::iterator> map_placeholders_to_values;
    std::map<std::string, std::list<std::string>> map_placeholders_to_list_of_possible_values;

    unsigned amount_of_placeholders;

    bool processed;

public:
    my_iterator(agent & a): my_agent{a}, amount_of_placeholders{0}, processed{false} {}

    void report(std::ostream & s) const;

    bool get_is_valid() const;
    bool get_finished() const;
    my_iterator& operator++();

    std::string get_current_name(std::string placeholder);

    void update(generic_name_item & source);

    unsigned get_amount_of_placeholders() const {
        return amount_of_placeholders;
    }
};

template <> constexpr const bool is_reportable<my_iterator> {true};
template <> constexpr const bool is_an_iterator<my_iterator> {true};


class input;
class output;

class expression
{
public:
    virtual ~expression() {}
    virtual void report(std::ostream & s) const = 0;
    virtual void update_iterator(my_iterator & target) = 0;

    /**
     * The output argument may be nullptr!
     */
    virtual bool get_can_be_evaluated(agent & a, belief & b, my_iterator & source, input & j, input * j2, output * o) = 0;
    
    virtual bool get_can_be_evaluated_given_assumption(agent & a, my_iterator & source) = 0;

    /**
     * The output argument may be nullptr!
     */
    virtual std::string get_evaluate(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) = 0;
    
    virtual std::string get_evaluate_given_assumption(agent & a, my_iterator & source) = 0;

    virtual void explain_evaluation(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) = 0;

    /**
     * b2 is optional (can be nullptr) for the belief in the future.
     * j2 is optional (can be nullptr) for the input in the future.
     */
    virtual void assert_is_equal(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, const std::string v) = 0;

    virtual void assert_is_different(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, const std::string v) = 0;

    virtual bool get_contains_output_variables(agent & a, belief & b, my_iterator & source, input & j) const = 0;

    virtual void update_set_of_possible_values(set_of_possible_values & target, agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) = 0;

    virtual void set_possible_values(set_of_possible_values & st, agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2) = 0;
};

template <> constexpr const bool is_reportable<expression> {true};


class expression_variable_initial_value: public expression
{
private:
    std::shared_ptr<generic_name> my_variable_name;
public:
    expression_variable_initial_value(std::shared_ptr<generic_name> && n): my_variable_name{std::move(n)} {}

    virtual void report(std::ostream & s) const override;

    virtual void update_iterator(my_iterator & target) override;

    virtual bool get_can_be_evaluated(agent & a, belief & b, my_iterator & source, input & j, input * j2, output * o) override;
    
    virtual bool get_can_be_evaluated_given_assumption(agent & a, my_iterator & source) override;

    virtual std::string get_evaluate(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;
    
    virtual std::string get_evaluate_given_assumption(agent & a, my_iterator & source) override;

    virtual void explain_evaluation(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;

    virtual void assert_is_equal(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, const std::string v) override;

    virtual void assert_is_different(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, const std::string v) override;

    virtual bool get_contains_output_variables(agent & a, belief & b, my_iterator & source, input & j) const override;

    virtual void update_set_of_possible_values(set_of_possible_values & target, agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;

    virtual void set_possible_values(set_of_possible_values & st, agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2) override;
};

template <> constexpr const bool is_reportable<expression_variable_initial_value> {true};

class expression_variable_terminal_value: public expression
{
private:
    std::shared_ptr<generic_name> my_variable_name;
public:
    expression_variable_terminal_value(std::shared_ptr<generic_name> && n): my_variable_name{std::move(n)} {}

    virtual void report(std::ostream & s) const override;

    virtual void update_iterator(my_iterator & target) override;

    virtual bool get_can_be_evaluated(agent & a, belief & b, my_iterator & source, input & j, input * j2, output * o) override;

    virtual bool get_can_be_evaluated_given_assumption(agent & a, my_iterator & source) override;
    
    virtual std::string get_evaluate(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;
    
    virtual std::string get_evaluate_given_assumption(agent & a, my_iterator & source) override;

    virtual void explain_evaluation(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;

    virtual void assert_is_equal(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, const std::string v) override;

    virtual void assert_is_different(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, const std::string v) override;

    virtual bool get_contains_output_variables(agent & a, belief & b, my_iterator & source, input & j) const override;

    virtual void update_set_of_possible_values(set_of_possible_values & target, agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;

    virtual void set_possible_values(set_of_possible_values & st, agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2) override;
};

template <> constexpr const bool is_reportable<expression_variable_terminal_value> {true};

class expression_value: public expression
{
private:
    std::shared_ptr<generic_name> my_value_name;
public:
    expression_value(std::shared_ptr<generic_name> && n): my_value_name{std::move(n)} {}

    virtual void report(std::ostream & s) const override;

    virtual void update_iterator(my_iterator & target) override;

    virtual bool get_can_be_evaluated(agent & a, belief & b, my_iterator & source, input & j, input * j2, output * o) override;

    virtual bool get_can_be_evaluated_given_assumption(agent & a, my_iterator & source) override;
    
    virtual std::string get_evaluate(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;
    
    virtual std::string get_evaluate_given_assumption(agent & a, my_iterator & source) override;

    virtual void explain_evaluation(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;

    virtual void assert_is_equal(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, const std::string v) override;

    virtual void assert_is_different(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, const std::string v) override;

    virtual bool get_contains_output_variables(agent & a, belief & b, my_iterator & source, input & j) const override;

    virtual void update_set_of_possible_values(set_of_possible_values & target, agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;

    virtual void set_possible_values(set_of_possible_values & st, agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2) override;
};

template <> constexpr const bool is_reportable<expression_value> {true};


class logical_expression
{
public:
    virtual ~logical_expression() {}

    virtual void report(std::ostream & s) const = 0;

    virtual void update_iterator(my_iterator & target) = 0;

    virtual bool get_evaluate(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) = 0;
    
    virtual bool get_evaluate_given_assumption(agent & a, my_iterator & source) = 0;

    virtual void explain_evaluation(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) = 0;

    virtual bool get_can_be_evaluated(agent & a, belief & b, my_iterator & source, input & j, input * j2, output * o) = 0;
    
    virtual bool get_can_be_evaluated_given_assumption(agent & a, my_iterator & source) = 0;

    /**
     * assert_is_true may change the agent's belief so that the logical expression is true.
     */
    virtual void assert_is_true(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, bool & assertion_failed) = 0;

    virtual void assert_is_false(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, bool & assertion_failed) = 0;

    virtual bool get_contains_output_variables(agent & a, belief & b, my_iterator & source, input & j) const = 0;

};

template <> constexpr const bool is_reportable<logical_expression> {true};


class logical_expression_and: public logical_expression
{
private:
    std::shared_ptr<logical_expression> left, right;
public:
    logical_expression_and(std::shared_ptr<logical_expression> && l, std::shared_ptr<logical_expression> && r): left{std::move(l)}, right{std::move(r)} {}

    virtual void report(std::ostream & s) const override
    {
        left->report(s);
        s << " && ";
        right->report(s);
    }

    virtual void update_iterator(my_iterator & target) override
    {
        left->update_iterator(target);
        right->update_iterator(target);
    }

    virtual bool get_evaluate(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override
    {
        return left->get_evaluate(a, b, b2, source, j, j2, o) && right->get_evaluate(a, b, b2, source, j, j2, o);
    }
    
    virtual bool get_evaluate_given_assumption(agent & a, my_iterator & source) override
    {
        return left->get_evaluate_given_assumption(a, source) && right->get_evaluate_given_assumption(a, source);
    }

    virtual void explain_evaluation(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override
    {
        left->explain_evaluation(a, b, b2, source, j, j2, o);
        right->explain_evaluation(a, b, b2, source, j, j2, o);
        std::cout << "so ";
        left->report(std::cout);
        std::cout << " && ";
        right->report(std::cout);
        std::cout << " = " << (left->get_evaluate(a, b, b2, source, j, j2, o) && right->get_evaluate(a, b, b2, source, j, j2, o) ? "true" : "false") << "\n";
    }

    virtual bool get_can_be_evaluated(agent & a, belief & b, my_iterator & source, input & j, input * j2, output * o) override;
    
    virtual bool get_can_be_evaluated_given_assumption(agent & a, my_iterator & source) override;

    virtual void assert_is_true(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, bool & assertion_failed) override
    {
        bool left_failed, right_failed;
        left->assert_is_true(a, b, b2, source, j, j2, left_failed);
        right->assert_is_true(a, b, b2, source, j, j2, right_failed);
        assertion_failed = left_failed || right_failed;
    }

    virtual void assert_is_false(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, bool & assertion_failed) override
    {
        assertion_failed = false;
    }

    virtual bool get_contains_output_variables(agent & a, belief & b, my_iterator & source, input & j) const override
    {
        return left->get_contains_output_variables(a,b,source,j) || right->get_contains_output_variables(a,b,source,j);
    }
};


template <> constexpr const bool is_reportable<logical_expression_and> {true};


class logical_expression_or: public logical_expression
{
private:
    std::shared_ptr<logical_expression> left, right;
public:
    logical_expression_or(std::shared_ptr<logical_expression> && l, std::shared_ptr<logical_expression> && r): left{std::move(l)}, right{std::move(r)} {}
    virtual void report(std::ostream & s) const override
    {
        left->report(s);
        s << " || ";
        right->report(s);
    }
    virtual void update_iterator(my_iterator & target) override
    {
        left->update_iterator(target);
        right->update_iterator(target);
    }
    virtual bool get_evaluate(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override
    {
        return left->get_evaluate(a, b, b2, source, j, j2, o) || right->get_evaluate(a, b, b2, source, j, j2, o);
    }
    virtual bool get_evaluate_given_assumption(agent & a, my_iterator & source) override
    {
        return left->get_evaluate_given_assumption(a, source) || right->get_evaluate_given_assumption(a, source);
    }
    virtual void explain_evaluation(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override
    {
        left->explain_evaluation(a, b, b2, source, j, j2, o);
        right->explain_evaluation(a, b, b2, source, j, j2, o);
        std::cout << "so ";
        left->report(std::cout);
        std::cout << " || ";
        right->report(std::cout);
        std::cout << " = " << (left->get_evaluate(a, b, b2, source, j, j2, o) || right->get_evaluate(a, b, b2, source, j, j2, o) ? "true" : "false") << "\n";
    }
    virtual bool get_can_be_evaluated(agent & a, belief & b, my_iterator & source, input & j, input * j2, output * o) override
    {
        return left->get_can_be_evaluated(a, b, source, j, j2, o) && right->get_can_be_evaluated(a, b, source, j, j2, o);
    }
    virtual bool get_can_be_evaluated_given_assumption(agent & a, my_iterator & source) override
    {
        return left->get_can_be_evaluated_given_assumption(a, source) && right->get_can_be_evaluated_given_assumption(a, source);
    }
    
    virtual void assert_is_true(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, bool & assertion_failed) override
    {
        assertion_failed = false;
    }
    virtual void assert_is_false(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, bool & assertion_failed) override
    {
        bool left_failed, right_failed;
        left->assert_is_false(a, b, b2, source, j, j2, left_failed);
        right->assert_is_false(a, b, b2, source, j, j2, right_failed);
        assertion_failed = left_failed || right_failed;
    }
    virtual bool get_contains_output_variables(agent & a, belief & b, my_iterator & source, input & j) const override
    {
        return left->get_contains_output_variables(a,b,source,j) || right->get_contains_output_variables(a,b,source,j);
    }
};

template <> constexpr const bool is_reportable<logical_expression_or> {true};


class logical_expression_equality: public logical_expression
{
private:
    std::shared_ptr<expression> left, right;
public:
    logical_expression_equality(std::shared_ptr<expression> && l, std::shared_ptr<expression> && r): left{std::move(l)}, right{std::move(r)} {}
    virtual void report(std::ostream & s) const override
    {
        left->report(s);
        s << " == ";
        right->report(s);
    }
    virtual void update_iterator(my_iterator & target) override
    {
        left->update_iterator(target);
        right->update_iterator(target);
    }
    virtual bool get_can_be_evaluated(agent & a, belief & b, my_iterator & source, input & j, input * j2, output * o) override;
    
    virtual bool get_can_be_evaluated_given_assumption(agent & a, my_iterator & source) override;

    virtual bool get_evaluate(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;
    
    virtual bool get_evaluate_given_assumption(agent & a, my_iterator & source) override
    {
        return left->get_evaluate_given_assumption(a, source) == right->get_evaluate_given_assumption(a, source);
    }

    virtual void explain_evaluation(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;

    virtual void assert_is_true(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, bool & assertion_failed) override;

    virtual void assert_is_false(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, bool & assertion_failed) override;

    virtual bool get_contains_output_variables(agent & a, belief & b, my_iterator & source, input & j) const override
    {
        return left->get_contains_output_variables(a,b,source,j) || right->get_contains_output_variables(a,b,source,j);
    }
};

template <> constexpr const bool is_reportable<logical_expression_equality> {true};


class logical_expression_not: public logical_expression
{
private:
    std::shared_ptr<logical_expression> inner;
public:
    logical_expression_not(std::shared_ptr<logical_expression> && i): inner{std::move(i)} {}

    virtual void report(std::ostream & s) const override
    {
        s << "! ";
        inner->report(s);
    }
    virtual void update_iterator(my_iterator & target) override
    {
        inner->update_iterator(target);
    }
    virtual bool get_evaluate(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override
    {
        return !inner->get_evaluate(a, b, b2, source, j, j2, o);
    }
    virtual bool get_evaluate_given_assumption(agent & a, my_iterator & source) override
    {
        return !inner->get_evaluate_given_assumption(a, source); 
    }
    virtual bool get_can_be_evaluated_given_assumption(agent & a, my_iterator & source) override
    {
        return inner->get_can_be_evaluated_given_assumption(a, source);
    }
    virtual void explain_evaluation(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override
    {
        inner->explain_evaluation(a, b, b2, source, j, j2, o);
        std::cout << "so !";
        inner->report(std::cout);
        std::cout << "=" << (!inner->get_evaluate(a, b, b2, source, j, j2, o) ? "true" : "false") << "\n";
    }

    virtual bool get_can_be_evaluated(agent & a, belief & b, my_iterator & source, input & j, input * j2, output * o) override
    {
        return inner->get_can_be_evaluated(a, b, source, j, j2, o);
    }

    virtual void assert_is_true(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, bool & assertion_failed) override
    {
        inner->assert_is_false(a, b, b2, source, j, j2, assertion_failed);
    }

    virtual void assert_is_false(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, bool & assertion_failed) override
    {
        inner->assert_is_true(a, b, b2, source, j, j2, assertion_failed);
    }

    virtual bool get_contains_output_variables(agent & a, belief & b, my_iterator & source, input & j) const override
    {
        return inner->get_contains_output_variables(a,b,source,j);
    }
};


template <> constexpr const bool is_reportable<logical_expression_not> {true};

/**
 * Statements are used in the rules (as an action triggered when a condition is fulfilled.
 */
class statement
{
public:
    virtual ~statement() {}
    virtual void execute(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) = 0;
    virtual void execute_given_assumption(agent & a, my_iterator & source, bool & impossible_flag) {}
    
    virtual void report(std::ostream & s) const = 0;
    virtual void update_iterator(my_iterator & target) = 0;
};

template <> constexpr const bool is_reportable<statement> {true};

class statement_assert_is_true: public statement
{
private:
    std::shared_ptr<logical_expression> my_expression;
public:
    statement_assert_is_true(std::shared_ptr<logical_expression> && e): my_expression{std::move(e)} {}
    virtual void execute(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;
    virtual void report(std::ostream & s) const override;
    virtual void update_iterator(my_iterator & target) override
    {
        my_expression->update_iterator(target);
    }
};

template <> constexpr const bool is_reportable<statement_assert_is_true> {true};

class statement_assert_is_false: public statement
{
private:
    std::shared_ptr<logical_expression> my_expression;
public:
    statement_assert_is_false(std::shared_ptr<logical_expression> && e): my_expression{std::move(e)} {}
    virtual void execute(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;
    virtual void report(std::ostream & s) const override;
    virtual void update_iterator(my_iterator & target) override
    {
        my_expression->update_iterator(target);
    }
};

template <> constexpr const bool is_reportable<statement_assert_is_false> {true};


class statement_impossible: public statement
{
public:
    virtual void execute(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;
    virtual void report(std::ostream & s) const override;
    virtual void update_iterator(my_iterator & target) override {}
    virtual void execute_given_assumption(agent & a, my_iterator & source, bool & impossible_flag) { impossible_flag = true; }
};

template <> constexpr const bool is_reportable<statement_impossible> {true};

class statement_illegal: public statement
{
public:
    virtual void execute(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;
    virtual void report(std::ostream & s) const override;
    virtual void update_iterator(my_iterator & target) override {}
};

template <> constexpr const bool is_reportable<statement_illegal> {true};


class statement_add_payoff: public statement
{
private:
    const float p;
public:
    statement_add_payoff(float np): p{np} {}
    virtual void execute(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;
    virtual void report(std::ostream & s) const override;
    virtual void update_iterator(my_iterator & target) override {}
};

template <> constexpr const bool is_reportable<statement_add_payoff> {true};


class statement_there_is_a_chance: public statement
{
private:
    const float probability;
public:
    statement_there_is_a_chance(float np): probability{np} {}
    virtual void execute(agent & a, belief & b, belief * b2, my_iterator & source, input & j, input * j2, output * o) override;
    virtual void report(std::ostream & s) const override;
    virtual void update_iterator(my_iterator & target) override
    {}
};

template <> constexpr const bool is_reportable<statement_there_is_a_chance> {true};

/**
 * \class command
 * This is a base class for various commands that can be passed to the Triglav interpreter.
 */
class command
{
public:
    virtual ~command() {}
    virtual void execute(agent & a) const = 0;
    
    virtual bool get_is_consider_command() const { return false; }
    virtual void set_rank(int r) {}
};


class command_precalculate: public command
{
public:
    command_precalculate() {}
    virtual void execute(agent & a) const override;
};

class command_consider: public command
{
private:
    int my_rank;
    const std::string variable_name;
public:
    command_consider(const std::string & vn): variable_name{vn}, my_rank{0} {}
    virtual void execute(agent & a) const override;
    
    virtual bool get_is_consider_command() const override { return true; }
    
    virtual void set_rank(int r) override { my_rank = r; }
};

class command_expand: public command
{
private:
    const unsigned depth;
public:
    command_expand(unsigned d): depth{d} {}
    virtual void execute(agent & a) const override;
};

class command_loop: public command
{
private:
    const unsigned depth;
public:
    command_loop(unsigned d): depth{d} {}
    virtual void execute(agent & a) const override;
};


class command_report: public command
{
public:
    virtual void execute(agent & a) const override;
};

class command_estimate_cases: public command
{
public:
    command_estimate_cases() {}
    virtual void execute(agent & a) const override;
};

class command_generate_cases: public command
{
private:        
public:
    command_generate_cases() {}
    virtual void execute(agent & a) const override;
};

class command_save_databank: public command
{
private:
    const std::string databank_location;    
public:
    command_save_databank(const std::string & dl): databank_location{dl} {}
    virtual void execute(agent & a) const override;
};

class command_open_databank: public command
{
private:
    const std::string databank_location;
public:
    command_open_databank(const std::string & dl): databank_location{dl} {}
    virtual void execute(agent & a) const override;    
};


class output
{
private:
    agent & my_agent;

    bool illegal;

    class output_item
    {
    private:
        variable_instance & my_variable_instance;
        std::vector<std::shared_ptr<value_instance>>::iterator my_value_instance_iterator;
    public:
        output_item(variable_instance & vi1, std::vector<std::shared_ptr<value_instance>>::iterator vi2);

        const variable_instance & get_variable_instance() const {
            return my_variable_instance;
        }
        variable_instance & get_variable_instance() {
            return my_variable_instance;
        }

        const value_instance & get_value_instance() const {
            return **my_value_instance_iterator;
        }
        std::vector<std::shared_ptr<value_instance>>::iterator & get_value_instance_iterator() {
            return my_value_instance_iterator;
        }
        const std::vector<std::shared_ptr<value_instance>>::iterator get_value_instance_iterator() const {
            return my_value_instance_iterator;
        }

        void report(std::ostream & s) const;
    };

    using inner_class = output_item;

    std::vector<std::shared_ptr<output_item>> vector_of_output_items;
public:
    output(agent & a);

    bool get_all_actions_have_been_processed() const;

    bool get_is_valid() const;

    output& operator++();

    void report(std::ostream & s) const;

    std::string get_variable_instance_value(const std::string i) const;


    void set_legal() {
        illegal = false;
    }

    void set_illegal() {
        illegal = true;
    }

    bool get_illegal() const {
        return illegal;
    }

    output& operator=(output & source);
};

template <> constexpr const bool is_reportable<output> {true};
template <> constexpr const bool is_an_iterator<output> {true}; // not really but it resembles an iterator
template <> constexpr const bool has_a_vector_of_inner_class_instances<output> {true};

/**
 * \class input
 * This class instance is populated when reading an input and it represents the values of the input variables.
 */
class input
{
private:
    float payoff, probability;

    agent & my_agent;


    /**
     * An input item is an answer to a single input question (what is the value of a particular input variable).
     */
    class input_item
    {
    private:
        variable_instance & my_variable_instance;
        value_instance * my_value_instance;
    public:
        input_item(variable_instance & vi1, value_instance * vi2): my_variable_instance{vi1}, my_value_instance{vi2} {}

        const variable_instance & get_variable_instance() const {
            return my_variable_instance;
        }
        const value_instance & get_value_instance() const {
            return *my_value_instance;
        }

        void all_but_one_state_is_impossible(const std::string & v);

        void reset();

        void report(std::ostream & s) const;

        bool get_is_defined() const {
            return my_value_instance!=nullptr;
        }
    };

    using inner_class = input_item;

    std::string line, question;
    std::vector<std::shared_ptr<input_item>> vector_of_input_items;

public:
    /**
     * This string is returned when a value is not defined for a variable.
     */
    static const std::string unknown_marker;

    std::string& get_line() {
        return line;
    }

    input(agent & a);

    void create_all_input_items();

    const std::string & get_question() const {
        return question;
    }

    /**
     * This function is not const - it sets the question (if necessary).
     */
    bool get_all_questions_have_been_answered();

    bool get_question_has_been_answered(const std::string & q) const;

    void parse_line();

    std::string get_variable_instance_value(const std::string i) const;

    void reset_payoff() {
        payoff = 0.0f;
    }
    void add_payoff(float p) {
        payoff += p;
    }
    float get_payoff() const {
        return payoff;
    }

    void reset_probability() {
        probability = 0.0f;
    }
    void set_probability(float p) {
        probability = p;
    }
    float get_probability() const {
        return probability;
    }

    void all_but_one_state_is_impossible(variable_instance & i, const std::string & v);

    void there_is_a_chance(float p);

    void report(std::ostream & s) const;

    std::vector<std::shared_ptr<input_item>> & get_vector_of_input_items() {
        return vector_of_input_items;
    }
};

template <> constexpr const bool is_reportable<input> {true};
template <> constexpr const bool has_a_vector_of_inner_class_instances<input> {true};


class rule
{
private:
    std::shared_ptr<logical_expression> condition;
    std::shared_ptr<statement> implication;
    unsigned line_number;
    std::string comment;
public:
    rule(std::shared_ptr<logical_expression> && c, std::shared_ptr<statement> && i, unsigned l, std::string & cm):
        condition{std::move(c)}, implication{std::move(i)}, line_number{l}, comment{cm} {}

    void report(std::ostream & s) const
    {
        s << comment << ";\n";
        s << "if (";
        condition->report(s);
        s << ") {";
        implication->report(s);
        s << "}";
    }

    bool try_to_apply_as_a_static_rule(agent & a, belief & b, input & j, input * j2);
    bool try_to_apply_as_a_dynamic_rule(agent & a, belief & b, belief * b2, input & j, input * j2, output & o);
    bool try_to_apply_as_assumption_rule(agent & a, bool & impossible_flag);
    

    /**
     * The dynamic rules contain at least one output variable (initial value).
     */
    bool get_is_dynamic(agent & a, belief & b, my_iterator & source, input & j) const;

    const std::string get_comment() const { return comment; }
};

template <> constexpr const bool is_reportable<rule> {true};

/**
 * This class is instantiated by the XML parser when opening a databank.
 * It represents a family of precalculated files for all processors.
 */
class precalculated_files
{
private:
    bool done;
    std::string prefix;
    unsigned depth;
public:
    precalculated_files(unsigned ndepth, const std::string & nprefix, bool ndone):
        done{ndone},
        prefix{nprefix},
        depth{ndepth}
    {
    }
    
    virtual ~precalculated_files() {}
    
    bool get_done() const { return done; }
    const std::string & get_prefix() const { return prefix; }
    unsigned get_depth() const { return depth; }
};


/**
 * \class agent
 * This is the main optimizer class. It uses a cpp_parser to parse a Triglav file and then it can execute the commands.
 */
class agent
{
public:

    constexpr static unsigned amount_of_repetitions=2;

    /**
     * \class my_iterator_for_types
     * This iterator is used when expanding the types. It generates the type instances.
     */
    class my_iterator_for_types: public my_iterator
    {
    private:
        unsigned level;
        std::shared_ptr<generic_name> & name_being_expanded;

    public:
        my_iterator_for_types(agent & a, std::shared_ptr<generic_name> & n, unsigned l);

    };

    /**
     * \class my_iterator_for_variables
     * This iterator is used when expanding the variables. It generates the variable instances.
     */
    class my_iterator_for_variables: public my_iterator
    {
    private:
        generic_name & name_being_expanded;
    public:
        my_iterator_for_variables(agent & a, generic_name & n);
    };


    class my_iterator_for_rules: public my_iterator
    {
    private:
        rule & my_rule;
    public:
        my_iterator_for_rules(agent & a, rule & r);
    };
    
    
    /**
     * \class my_iterator_for_estimating_variable_instances
     * This class is NOT inherited from my_iterator, although it is somewhat similar.
     * It is used to estimate all the cases (i.e. tuples (visible_state, state)).
     */
    class my_iterator_for_estimating_variable_instances
    {
    protected:
        agent & my_agent;

        /**
         * This flag indicates that the iterator has completed its run.
         */
        bool processed;

        /**
         * This attribute equals the amount of all input variable instances and hidden variable instance.
         */
        unsigned amount_of_variable_instances;
                
        /**
         * The max amount of unusual values.
         */
        unsigned depth;

        /**
         * \class vector_of_indices_and_some_other_stuff
         * This class resembles an iterator, it contains a vector of indices denoting the variable instances
         * that may have unusual values.
         */
        class vector_of_indices_and_some_other_stuff
        {
        private:
            unsigned amount_of_variable_instances;
            
            /**
             * The max amount of unusual values.
             */            
            unsigned depth;
            
            /**
             * A vector of d numbers (d - depth) that indicate which variable instances can have unusual values.
             */
            std::vector<int> vector_of_indices_can_be_unusual;
            
            bool finished;
                    
        public:
            vector_of_indices_and_some_other_stuff(unsigned d);
            
            void set_amount_of_variable_instances(unsigned a) { amount_of_variable_instances = a; }
            
            bool get_finished() const;
            
            void report(std::ostream & s) const;
            
            bool get_allows_unusual_values_at(unsigned index) const;                        
            
            vector_of_indices_and_some_other_stuff& operator++();
            
        } my_vector_of_indices;
        

        std::vector<std::string> vector_of_variable_instances_names;

    public:
        
        my_iterator_for_estimating_variable_instances(agent & a, unsigned d);
                
        bool get_finished() const;  // this function is not virtual!
        
        my_iterator_for_estimating_variable_instances& operator++();        
                
        void report(std::ostream & s) const;        
        
        agent & get_agent() const { return my_agent; }
        
        unsigned get_depth() const { return depth; }
        
        bool get_is_matching(const std::string & vr) const;
    };
    
    
    /**
     * \class my_iterator_for_variable_instances
     * Its base class is NOT inherited from my_iterator, although it is somewhat similar.
     * It is used to generate all the cases (i.e. tuples (visible_state, state)).
     */
    class my_iterator_for_variable_instances: public my_iterator_for_estimating_variable_instances
    {        
    public:        
        
        
        /**
         * \class list_of_possible_values_and_some_other_stuff
         * This is a small internal class used internally by this iterator. It contains a list of all values
         * and a list of usual values.
         */
        class list_of_possible_values_and_some_other_stuff
        {
        private:
            std::list<std::string> my_list, my_list_of_unusual_values;
            
            variable_mode my_mode;
            
            bool has_usual_values;          
            
            /**
             * This flag is true if (and only if) we have merely one usual value.
             * It helps to speed up the iterator incrementation.
             */
            bool has_exactly_one_usual_value;
            
            /**
             * index of this entry in the vector_of_variable_instances_names.
             */
            unsigned index;
            
        public:
            list_of_possible_values_and_some_other_stuff(variable_mode m, bool h, unsigned i): my_mode{m}, has_usual_values{h}, index{i} {}
            
            std::list<std::string>::iterator get_begin(bool unusual)
            {
                if (unusual)
                    return my_list_of_unusual_values.begin();
                return my_list.begin();
            }
            
            std::list<std::string>::iterator get_end(bool unusual)
            {
                if (unusual)
                    return my_list_of_unusual_values.end();
                return my_list.end();
            }
            
            const std::list<std::string>::const_iterator get_begin(bool unusual) const
            {
                if (unusual)
                    return my_list_of_unusual_values.begin();
                return my_list.begin();
            }
            
            const std::list<std::string>::const_iterator get_end(bool unusual) const
            {
                if (unusual)
                    return my_list_of_unusual_values.end();
                return my_list.end();
            }
            
            std::list<std::string>& get_list() { return my_list; }
            
            const std::list<std::string>& get_list() const { return my_list; }
            
            void add_unusual_value(const std::string & v) { my_list_of_unusual_values.push_back(v); }
            
            unsigned get_index() const { return index; }
            
            bool get_has_usual_values() const { return has_usual_values; }                        
            
            unsigned get_amount_of_unusual_values() const { return my_list_of_unusual_values.size(); }
            
            void set_has_exactly_one_usual_value(bool s) { has_exactly_one_usual_value = s; }
            
            bool get_has_exactly_one_usual_value() const { return has_exactly_one_usual_value; }
            
            void report(std::ostream & s) const;
        };
        
    private:
        
    bool allows_unusual_values;
    bool has_exactly_one_usual_value;
    bool incremented;
    std::string current_variable_instance_name;
    
    void on_exactly_one_usual_value(std::map<std::string, std::list<std::string>::iterator>::iterator & i, unsigned & r);
    
    void on_regular_iteration(std::map<std::string, std::list<std::string>::iterator>::iterator i, unsigned & r);
    
    void on_end_of_regular_iteration(std::map<std::string, std::list<std::string>::iterator>::iterator i, unsigned & r);
    
    
    /**
     * Ask the agent to assume that many variable instances values
     * and check whether they do not violate the rules.
     */
    bool get_is_assumed_ok(unsigned r);
    
    void partial_reinitialize(unsigned r);
    
    /**
     * This function is executed by the constructor to calculate
     * how many of the variable instances are correct.
     */
    void calculate_amount_of_checked_variable_instances();
    
    
    void initialize_list_item_if_it_has_usual_values(std::list<std::shared_ptr<variable_instance>>::iterator i, std::map<std::string, list_of_possible_values_and_some_other_stuff>::iterator it);
    
    void initialize_list_item_if_it_has_no_usual_values(std::list<std::shared_ptr<variable_instance>>::iterator i, std::map<std::string, list_of_possible_values_and_some_other_stuff>::iterator it);
    
    /**
     * This function is only used by the constructor. It initializes the list of possible values.
     */
    void initialize_list_item(std::list<std::shared_ptr<variable_instance>>::iterator i, std::map<std::string, list_of_possible_values_and_some_other_stuff>::iterator it);

        
    protected:
        
        std::map<std::string, std::list<std::string>::iterator> map_variable_instances_to_values, map_variable_instances_to_end_iterator;
        
        std::map<std::string, list_of_possible_values_and_some_other_stuff> map_variable_instances_to_list_of_possible_values;
                
        /**
         * This number indicates how many variable instances (beginning from the first one in the order determined by vector_of_variable_instances_names)
         * have been checked according to the rules.
         */
        unsigned amount_of_checked_variable_instances;
                        
    protected:
        void reinitialize();
        
    public:
        my_iterator_for_variable_instances(agent & a, unsigned d);
        
        bool get_is_partially_not_end(unsigned n);
        
        bool get_is_valid() const;
        bool get_finished() const;
        
        my_iterator_for_variable_instances& operator++();        
        
        void report(std::ostream & s) const;
        
        void partial_report(std::ostream & s) const;
        
        std::string get_value(const std::string & n) const;
    };
    
    /**
     * This iterator works for a single validation range.
     */
    class my_single_range_iterator_for_variable_instances: public my_iterator_for_variable_instances
    {
    private:
        const std::string my_range;
        
        std::list<std::string>::iterator my_begin_unusual, my_begin_usual, my_end_unusual, my_end_usual;
        
        void init_for_given_amount_of_checked_variable_instances(unsigned &r, 
                bool allows_unusual_values, 
                bool has_exactly_one_usual_value, 
                std::map<std::string, std::list<std::string>::iterator>::iterator & i,
                const std::string & variable_name, bool & continue_flag);
        
        
        void init_for_unusual_value(unsigned & r, std::map<std::string, std::list<std::string>::iterator>::iterator & i, const std::string & variable_name, bool & continue_flag);
        
        void init_for_single_usual_value(unsigned & r, std::map<std::string, std::list<std::string>::iterator>::iterator & i, const std::string & variable_name, bool & continue_flag);
        
        void init_for_regular_value(unsigned & r, std::map<std::string, std::list<std::string>::iterator>::iterator & i, const std::string & variable_name, bool & continue_flag);
        
        void init_for_end(unsigned & r, std::map<std::string, std::list<std::string>::iterator>::iterator & i, const std::string & variable_name, bool & continue_flag);

    public:
        my_single_range_iterator_for_variable_instances(const my_iterator_for_estimating_variable_instances & i, const std::string & range);

        void report(std::ostream & s) const;
        
        bool get_is_valid() const;
        
        my_single_range_iterator_for_variable_instances& operator++();        

    };
    
    /**
     * \class my_output_multifile
     * This class creates a vector of output files named "case_<number>.txt" and is able to choose randomly one of them.
     * It is used by the agent when generating cases.
     */
    class my_output_multifile
    {
    private:
        class output_file
        {
        private:
            std::ofstream output_file_stream;
            unsigned number;
            int processor_id;
        public:
            output_file(const std::string & path, const std::string & prefix, unsigned n, const std::string & extension, int proc_id);
            ~output_file();
            
            std::ofstream & get_stream() { return output_file_stream; }
        };
    
        std::vector<std::shared_ptr<output_file>> vector_of_output_files;                        
        std::random_device dev;
        std::mt19937 rng;
        std::uniform_int_distribution<std::mt19937::result_type> dist;
        int processor_id;
        
    public:                
        my_output_multifile(const std::string & path, const std::string & prefix, unsigned amount_of_files, const std::string & extension, int proc_id);
        
        std::ofstream & get_random_output_file_stream();        
    };
    
private:
    
    /**
     * This multifile is used when we generate cases.
     */
    std::shared_ptr<my_output_multifile> my_multifile;
    
    /**
     * The consider rank determines the order in which we consider the variables when generating cases.
     */
    std::map<std::string, int> map_variable_instances_to_consider_rank;
    
    /**
     * This number is increased for each consider command, then the variable instances are sorted
     * when generating cases according to this rank growing. The max value is assigned to all 
     * variable instances that are not "considered" by the user explicitly.
     */
    int rank;
    
    int get_variable_instance_rank(const std::string & n) const;
    
    void assume_only_the_first_n_variable_instance_known(unsigned n, const std::vector<std::string> & v);
    
    bool get_the_iterator_is_partially_valid(unsigned n, my_iterator_for_variable_instances & i);            
    

private:

    struct class_with_scanner
    {
        void * scanner;
        void * agent_ptr;
    };

private:    
    
    std::list<std::shared_ptr<type>> list_of_types;
    std::list<std::shared_ptr<variable>> list_of_variables;
    std::list<std::shared_ptr<type_instance>> list_of_type_instances;
    std::list<std::shared_ptr<variable_instance>> list_of_variable_instances;
    std::list<std::shared_ptr<rule>> list_of_rules;
    std::list<std::shared_ptr<value_instance>> list_of_value_instances;
    std::list<std::shared_ptr<command>> list_of_commands;
    std::vector<std::unique_ptr<precalculated_files>> vector_of_precalculated_files;

    std::shared_ptr<belief> current_belief;

    std::map<std::string,std::shared_ptr<variable_instance>> map_name_to_variable_instance;
    std::map<std::string,std::shared_ptr<value_instance>> map_name_to_value_instance;        

    
    std::vector<std::pair<std::string, int>> vector_of_validation_items;

    bool debug;
    unsigned amount_of_actions;
    unsigned depth;        
    unsigned max_amount_of_unusual_cases;
    unsigned amount_of_processors, amount_of_case_files;
    std::string case_files_prefix, case_files_extension, case_files_path;
    
    /**
     * This is a map used to convert a variable instance name to a boolean flag denoting whether it is assumed.
     */
    std::map<std::string, bool> map_name_to_assumed_flag;
    
    std::map<std::string, std::string> map_name_to_assumed_value;

    void internal_expand_for_types(unsigned i);
    void internal_expand_for_values();
    void internal_expand_for_variables();

    void populate_belief_for_consequence(belief & b, input & j, input * j2, output & o, belief & target);
    float get_payoff_expected_value_for_consequences(belief & b, unsigned depth, input & j, output & o);

    using time_in_seconds = decltype(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now()-std::chrono::system_clock::now()));

    /**
     * \param b input parameter, a belief reference denoting the current belief
     * \param depth input parameter, the height of the game tree
     * \param j input parameter, the input data
     * \param decision output parameter, the optimal action chosen by the algorithm
     * \param elapsed output argument, the time in seconds elapsed
     * 
     * The function get_optimal_action is denoted as \f$\alpha:B \times N \to A\f$ in the documentation.
     */
    void get_optimal_action(belief & b, unsigned depth, input & j, output & decision, time_in_seconds & elapsed);

protected:

    virtual void get_input(input & i);

    int parse_error_to_stderr(class_with_scanner * data);
        
    void precalculate_for_processor(int processor_id);
    
    void generate_cases_for_processor(int processor_id);
    
public:
    
    agent();
        
    void add_consider_variable_instance(const std::string & n, int rank);
    
    bool get_is_assumed(const std::string & n) const;
    
    std::string get_assumed_value(const std::string & n) const;
    
    /**
     * This method is called by the cpp_parser, it calculates the ranks of the "consider commands", i.e. the first
     * one gets 0, the next one 1, 2, and so on. If a variable instance is not "considered" then it gets maximum rank.
     * Later, when generating cases, we use these ranks to sort the variable instances.
     */
    void calculate_the_consider_ranks();

    
    void set_amount_of_case_files(unsigned a)
    {
        amount_of_case_files = a;
    }
    
    void set_case_files_prefix(const std::string & p)
    {
        case_files_prefix = p;
    }
    
    void set_case_files_extension(const std::string & e)
    {
        case_files_extension = e;
    }
    
    void set_case_files_path(const std::string & p)
    {
        case_files_path = p;
    }
    
    
    unsigned get_amount_of_case_files() const
    {
        return amount_of_case_files;
    }
    
    const std::string & get_case_files_prefix() const
    {
        return case_files_prefix;
    }
    
    const std::string & get_case_files_extension() const
    {
        return case_files_extension;
    }
    
    const std::string & get_case_files_path() const
    {
        return case_files_path;
    }
    
    
    void set_max_amount_of_unusual_cases(unsigned m)
    {
        max_amount_of_unusual_cases = m;
    }
    
    void set_amount_of_processors(unsigned a)
    {
        amount_of_processors = a;
    }
    
    bool get_debug() const {
        return debug;
    }

    void set_debug(bool d) {
        debug = d;
    }

    void add_type(std::shared_ptr<type> && t)
    {
        list_of_types.push_back(std::move(t));
    }
    void add_command(std::shared_ptr<command> && c)
    {
        list_of_commands.push_back(std::move(c));
    }
    void add_variable(std::shared_ptr<variable> && v)
    {
        list_of_variables.push_back(std::move(v));
    }
    void add_variable_instance(std::shared_ptr<variable_instance> && v);
    void add_rule(std::shared_ptr<rule> && r)
    {
        list_of_rules.push_back(std::move(r));
    }
    void report(std::ostream & s) const;

    void report_input_variables(std::ostream & s) const;

    void expand(unsigned depth);

    void loop(unsigned depth);
    
    void generate_cases();
    
    void precalculate();

    void estimate_cases();
    
    void clear_validation_items();
    
    /**
     * Adds a validation item with the processor id to the list.
     * The processor id begins with 0.
     */
    void add_validation_item(const std::string & v, int processor_id);
    
    void execute();

    std::list<std::shared_ptr<variable_instance>> & get_list_of_variable_instances() {
        return list_of_variable_instances;
    }
    std::list<std::shared_ptr<type_instance>> & get_list_of_type_instances() {
        return list_of_type_instances;
    }
    std::list<std::shared_ptr<value_instance>> & get_list_of_value_instances() {
        return list_of_value_instances;
    }
    std::list<std::shared_ptr<type>> & get_list_of_types() {
        return list_of_types;
    }        
    std::vector<std::unique_ptr<precalculated_files>> & get_vector_of_precalculated_files() {
        return vector_of_precalculated_files;
    }
    
    std::map<std::string,std::shared_ptr<variable_instance>> & get_map_name_to_variable_instance() {
        return map_name_to_variable_instance;
    }
    std::map<std::string,std::shared_ptr<value_instance>> & get_map_name_to_value_instance() {
        return map_name_to_value_instance;
    }

    std::shared_ptr<belief> get_current_belief() {
        return current_belief;
    }

    int parse(const char * filename);
    
    void set_depth(unsigned d) { depth = d; }
    
    unsigned get_depth() const { return depth; }
};

template <> constexpr const bool is_an_iterator<agent::my_iterator_for_types> {true};
template <> constexpr const bool is_an_iterator<agent::my_iterator_for_variables> {true};
template <> constexpr const bool is_an_iterator<agent::my_iterator_for_rules> {true};
template <> constexpr const bool is_reportable<agent> {true};


/**
 * This is a special agent that gets a file name and reads an input file.
 */
class agent_reading_input_files: public agent
{
protected:
    virtual void get_input(input & i) override;
public:
};


template <> constexpr const bool is_reportable<agent_reading_input_files> {true};


class cpp_parser
{
private:
    struct token_and_name {
        int token;
        std::string name;
    };

    static const token_and_name array_of_tokens_and_names[];

    union lvalue {
        char * value_string;
        double value_float;
        int value_int;
    };

    lvalue yylval;

    void *scanner;

    agent & my_agent;

    bool unlex_done, continue_flag;

    int next_token;

    std::ostream *my_output_stream;

    std::string next_rule_comment;

    int lineno();

    int parse_rule();
    int parse_generic_name(std::shared_ptr<generic_name_list> & target);
    int parse_type();
    int parse_variable();
    int parse_logical_expression(std::shared_ptr<logical_expression> & target);
    int parse_logical_expression_and(std::shared_ptr<logical_expression> & target);
    int parse_logical_expression_not(std::shared_ptr<logical_expression> & target);
    int parse_logical_expression_equality(std::shared_ptr<logical_expression> & target);
    int parse_expression_variable_initial_value(std::shared_ptr<expression> & target);
    int parse_expression_variable_terminal_value(std::shared_ptr<expression> & target);
    int parse_expression_value(std::shared_ptr<expression> & target);
    int parse_expression(std::shared_ptr<expression> & target);
    int parse_statement(std::shared_ptr<statement> & target);
    int parse_expand();
    int parse_loop();
    int parse_report();
    int parse_generate_cases();
    int parse_estimate_cases();
    int parse_consider();
    int parse_precalculate();
    int parse_save_databank();
    int parse_open_databank();
public:
    cpp_parser(void * s, agent & a, std::ostream * os):
        unlex_done{false}, my_output_stream{os}, my_agent{a} {
        scanner = s;
    }

    int lex();

    void unlex(int);

    static const std::string get_token_name(int t);

    int parse();
};

/**
 * \class cases_parser
 * This class reuses the flex scanner to parse the cases_ files. These files contain all the possible states to be validated by the command
 * validate_cases. 
 */
class cases_parser
{
private:
    struct token_and_name {
        int token;
        std::string name;
    };

    static const token_and_name array_of_tokens_and_names[];
    
    void *scanner;

    agent & my_agent;
    
    std::ostream *my_output_stream;

    union lvalue {
        char * value_string;
        double value_float;
        int value_int;
    };

    lvalue yylval;
    
    int lineno();
public:
    cases_parser(void * s, agent & a, std::ostream * os):
        my_output_stream{os}, my_agent{a} 
    { 
        scanner = s;
    }

    int lex();
    
    int parse();
    
    static const std::string get_token_name(int t);
};

}


std::ostream & operator<<(std::ostream & s, const triglav::agent::my_iterator_for_variable_instances::list_of_possible_values_and_some_other_stuff & l);
