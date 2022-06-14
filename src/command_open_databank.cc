#include "triglav.h"
#include "config.h"
#include <sstream>
#ifdef TRIGLAV_XML_ON
#include <libxml/tree.h>
#include <libxml/parser.h>

class my_xml_document_databank
{
private:
    xmlDocPtr doc;
public:
    class iterator
    {
    private:
        xmlNode *current;
    public:
        iterator(xmlNode *root_element)
        {            
            current = root_element;
        }
        
        operator bool() const
        {
            return current != NULL;
        }
        
        iterator& operator++()
        {
            current = current->next;
            return *this;
        }
        
        xmlNode* operator*()
        {
            return current;
        }
    };
    
    class attribute_iterator
    {
    private:
        xmlAttr* attribute;
    public:
        attribute_iterator(iterator & i) { attribute = (*i)->properties; }

        operator bool() const
        {
            return attribute != NULL;
        }
        
        attribute_iterator& operator++()
        {
            attribute = attribute->next;
            return *this;
        }
        
        xmlAttr* operator*()
        {
            return attribute;
        }
    };        
    
    my_xml_document_databank(const std::string & filename)
    {
        doc = xmlReadFile(filename.c_str(), NULL, 0);
        if (doc == NULL) 
        {
            throw std::runtime_error("failed to open XML document databank");
        }
    }
    ~my_xml_document_databank()
    {
        xmlFreeDoc(doc);
    }
    
    iterator begin()
    {
        return iterator{xmlDocGetRootElement(doc)};
    }        
};


class databank_xml_parser
{
private:
    triglav::agent & my_agent;
    my_xml_document_databank & my_databank;
    
    class my_string
    {
    private:
        std::string value_string;
    public:
        my_string(my_xml_document_databank::iterator & node, my_xml_document_databank::attribute_iterator & a)
        {
            xmlChar* value;
            value = xmlNodeListGetString((*node)->doc, (*a)->children, 1);
            value_string = std::string{(char*)value};
            xmlFree(value);
        }
        std::string& operator*()
        {
            return value_string;
        }
    };

    
    void parse_variable_instance(my_xml_document_databank::iterator & instance);
    void parse_filename_schema(my_xml_document_databank::iterator & filename_schema);
    void parse_data(my_xml_document_databank::iterator & data);
    
public:
    databank_xml_parser(triglav::agent & a, my_xml_document_databank & d): my_agent{a}, my_databank{d} {}
    
    void parse();
};

void databank_xml_parser::parse_variable_instance(my_xml_document_databank::iterator & instance)
{
    for (my_xml_document_databank::attribute_iterator a{instance}; a; ++a)
    {
        my_string value{instance, a};
        //std::cout << (*a)->name << "=" << *value << "\n";                
    }
}


void databank_xml_parser::parse_filename_schema(my_xml_document_databank::iterator & filename_schema)
{    
    for (my_xml_document_databank::attribute_iterator a{filename_schema}; a; ++a)
    {
        my_string value{filename_schema, a};
        //std::cout << (*a)->name << "=" << *value << "\n";
        
        if (std::string((const char *)(*a)->name) == "amount_of_digits")
        {            
        }
    }
    
    for (my_xml_document_databank::iterator i{(*filename_schema)->children}; i; ++i)
    {
        if ((*i)->type == XML_ELEMENT_NODE)
        {
            std::string x{(const char *)(*i)->name};
            if (x == "input_variable_instance")
            {
                //std::cout << "input_variable_instance\n";                
                parse_variable_instance(i);
            }
        }
    }    
}


void databank_xml_parser::parse_data(my_xml_document_databank::iterator & data)
{
    unsigned processor=0;
    for (my_xml_document_databank::iterator k{(*data)->children}; k; ++k)
    {
        if ((*k)->type == XML_ELEMENT_NODE)
        {
            //std::cout << (*k)->name << "\n";
            std::string x{(const char *)(*k)->name};
                                
            if (x == "specification")
            {
                //std::cout << "parsing specification\n";
                for (my_xml_document_databank::iterator l{(*k)->children}; l; ++l)
                {
                    if ((*l)->type == XML_ELEMENT_NODE)
                    {
                        std::string x2{(const char *)(*l)->name};
                        if (x2 == "derived")
                        {
                            //std::cout << "parsing derived specification\n";
                        }
                    }
                }
            }
            else
            if (x == "case_files")
            {
                //std::cout << "parsing case_files\n";
                
                for (my_xml_document_databank::attribute_iterator a{k}; a; ++a)
                {                            
                    if (std::string((const char *)(*a)->name) == "amount")
                    {
                        my_string value{k, a};
                        std::stringstream s(*value);
                        unsigned amount;
                        s >> amount;
                        my_agent.set_amount_of_case_files(amount);
                    }
                    else 
                    if (std::string((const char *)(*a)->name) == "prefix")
                    {
                        my_string value{k, a};
                        my_agent.set_case_files_prefix(*value);
                    }
                    else
                    if (std::string((const char *)(*a)->name) == "extension")
                    {
                        my_string value{k, a};
                        my_agent.set_case_files_extension(*value);
                    }
                    else
                    if (std::string((const char *)(*a)->name) == "path")
                    {
                        my_string value{k, a};
                        my_agent.set_case_files_path(*value);
                    }                        
                }
                
                for (my_xml_document_databank::iterator l{(*k)->children}; l; ++l)
                {
                    if ((*l)->type == XML_ELEMENT_NODE)
                    {
                        std::string x2{(const char *)(*l)->name};
                        if (x2 == "precalculated_files")
                        {                            
                            unsigned depth=0;
                            bool done=false;
                            std::string prefix="";
                            
                            for (my_xml_document_databank::attribute_iterator a{l}; a; ++a)
                            {                            
                                if (std::string((const char *)(*a)->name) == "depth")
                                {
                                    my_string value{l, a};
                                    std::stringstream s{*value};
                                    s >> depth;
                                }
                                else 
                                if (std::string((const char *)(*a)->name) == "prefix")
                                {           
                                    my_string value{k, a};
                                    prefix = *value;
                                }
                                else 
                                if (std::string((const char *)(*a)->name) == "done")
                                {           
                                    my_string value{k, a};
                                    done = ((*value) == "true");
                                }
                            }                            
                            my_agent.get_vector_of_precalculated_files().push_back(std::make_unique<triglav::precalculated_files>(depth, prefix, done));
                        }
                    }
                }
                
            }
            else
            if (x == "generator")
            {
                for (my_xml_document_databank::attribute_iterator a{k}; a; ++a)
                {
                    if (std::string((const char *)(*a)->name) == "max_amount_of_unusual_values")
                    {
                        my_string value{k, a};
                        std::stringstream s(*value);
                        unsigned amount;
                        s >> amount;
                        my_agent.set_max_amount_of_unusual_cases(amount);
                    }
                }
            }
            else
            if (x == "validation_range")
            {
                for (my_xml_document_databank::iterator l{(*k)->children}; l; ++l)
                {
                    if ((*l)->type == XML_ELEMENT_NODE)
                    {
                        std::string x2{(const char *)(*l)->name};
                        if (x2 == "validation_item")
                        {
                            for (my_xml_document_databank::attribute_iterator a{l}; a; ++a)
                            {
                                if (std::string((const char *)(*a)->name) == "processor")
                                {
                                    my_string value{l, a};
                                    std::stringstream s(*value);
                                    s >> processor;
                                }
                            }
                            
                            for (my_xml_document_databank::iterator m{(*l)->children}; m; ++m)
                            {
                                if ((*m)->type == XML_TEXT_NODE)
                                {                                    
                                    std::string x3{(const char *)(*m)->content};       
                                    //std::cout << "range " << x3 << " procesor " << processor << "\n";
                                    my_agent.add_validation_item(x3, processor);
                                }
                            }                            
                        }
                    }                    
                }
            }
            else
            if (x == "machine")
            {
                for (my_xml_document_databank::attribute_iterator a{k}; a; ++a)
                {
                    my_string value{k, a};
                    //std::cout << (*a)->name << "=" << *value << "\n";
                                                                    
                    if (std::string((const char *)(*a)->name) == "amount_of_processors")
                    {
                        std::stringstream s{*value};
                        unsigned amount_of_processors;
                        s >> amount_of_processors;
                        my_agent.set_amount_of_processors(amount_of_processors);
                        
                        if (s.fail())
                        {
                            throw std::runtime_error("failed to parse amount_of_processors");
                        }
                        
                    }
                }
                
            }
            else
            if (x == "visible_states")
            {
                //std::cout << "parsing visible_states\n";
                for (my_xml_document_databank::iterator l{(*k)->children}; l; ++l)
                {
                    if ((*l)->type == XML_ELEMENT_NODE)
                    {
                        std::string x2{(const char *)(*l)->name};
                        if (x2 == "filename_schema")
                        {
                            //std::cout << "parsing filename_schema\n";
                            parse_filename_schema(l);
                        }
                    }
                }
            }
        }
    }    
}


void databank_xml_parser::parse()
{
    for (auto i{my_databank.begin()}; i; ++i)
    {
        if ((*i)->type == XML_ELEMENT_NODE)
        {
            //std::cout << (*i)->name << "\n";
            for (my_xml_document_databank::iterator j{(*i)->children}; j; ++j)
            {
                if ((*j)->type == XML_ELEMENT_NODE)
                {
                    //std::cout << (*j)->name << "\n";
                    
                    if (std::string((const char *)(*j)->name) == "data")
                    {
                        parse_data(j);
                    }                    
                }
            }
        }
    }
}

#endif

using namespace triglav;

void command_open_databank::execute(agent & a) const
{
#ifdef TRIGLAV_XML_ON    
    //std::cout << "open_databank " << databank_location << "\n";
    my_xml_document_databank r(databank_location);
    databank_xml_parser d{a, r};
    
    a.clear_validation_items();    
    d.parse();            
#endif    
}
