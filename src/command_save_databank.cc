#include "triglav.h"
#include "config.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#ifdef TRIGLAV_XML_ON
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#define MY_ENCODING "UTF-8"

class my_xml_writer
{
private:
    xmlTextWriterPtr writer;
    int rc;
    unsigned level;
public:
    my_xml_writer(const char * uri): level{0}
    {
        writer = xmlNewTextWriterFilename(uri, 0);
        if (!writer)
        {
            throw std::runtime_error("unable to open databank file for writing");
        }
    }
    
    void start_document()
    {
        rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
        if (rc < 0) {
            throw std::runtime_error("unable to start document");
        }
    }
    
    void write_text(const std::string & text)
    {
        rc = xmlTextWriterWriteFormatRaw(writer, "%s", BAD_CAST text.c_str());
        if (rc < 0) {
            throw std::runtime_error("unable to write text");
        }                
    }


    void start_element(const std::string & name)
    {
        if (level>0)
            write_text("\n");
        for (unsigned i=0; i<level; i++)
            write_text("    ");
        
        rc = xmlTextWriterStartElement(writer, BAD_CAST name.c_str());
        if (rc < 0) {
            throw std::runtime_error("unable to start element");
        }        
        level++;        
    }
    
    void write_attribute(const std::string & name, const std::string & value)
    {
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST name.c_str(),
                                     BAD_CAST value.c_str());
        if (rc < 0) {
            throw std::runtime_error("unable to start element");
        }        
    }
    
    void write_comment(const std::string & comment)
    {        
        if (level>0)
            write_text("\n");
        for (unsigned i=0; i<level; i++)
            write_text("    ");
        
        rc = xmlTextWriterWriteFormatComment(writer, "%s",comment.c_str());
        if (rc < 0) {
            throw std::runtime_error("unable to write comment");
        }
    }
            
    void end_element()
    {
        level--;
        write_text("\n");
        for (unsigned i=0; i<level; i++)
            write_text("    ");
        
        rc = xmlTextWriterEndElement(writer);
        if (rc < 0) {
            throw std::runtime_error("unable to end element");
        }        
    }
    
    ~my_xml_writer()
    {
        xmlFreeTextWriter(writer);
    }
};

#endif

using namespace triglav;

void command_save_databank::execute(agent & a) const
{
#ifdef TRIGLAV_XML_ON
    my_xml_writer w(databank_location.c_str());
    std::cout << "saving databank " << databank_location << "\n";
    
    w.start_document();
    w.start_element("triglav:databank");
    w.write_attribute("xmlns:triglav", "https://www.perkun.org/triglav/0.0.0");
    
    w.write_comment("Metadata about this document");
    w.start_element("triglav:meta");
    w.write_attribute("version", PACKAGE_VERSION);
    
    
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
    std::stringstream s0;
    s0 << (now->tm_year + 1900) << '-' 
         << std::setw(2) << std::setfill('0') << (now->tm_mon + 1) << '-'
         << std::setw(2) << std::setfill('0') << now->tm_mday;
                  
    w.write_attribute("date", s0.str());
    w.write_attribute("audience", "all");
    
    w.start_element("triglav:authors");
    w.start_element("triglav:author");
    w.write_attribute("name", "Your Name");
    w.write_attribute("email", "your@email.com");
    w.write_attribute("website", "https://your.website");
    w.write_attribute("type", "human");
    w.end_element(); // author
    w.end_element(); // authors
    w.end_element(); // meta

    w.write_comment("This element is intended for humans");    
    w.start_element("triglav:description");
    w.write_attribute("audience", "humans");
    w.write_text("This Triglav readme databank describes the files for the triglav precalculated knowledge. Please read the details on https://www.perkun.org.");
    w.end_element(); // description
    
    
    w.write_comment("This element is intended for machines");    
    w.start_element("triglav:data");
    w.write_attribute("audience", "machines");
    w.start_element("triglav:specification");
    w.start_element("triglav:origin");
    w.start_element("triglav:types");
    
    for (auto i{a.get_list_of_types().begin()}; i!=a.get_list_of_types().end(); i++)
    {
        w.start_element("triglav:type");
        w.write_attribute("name", (*i)->get_name());
        w.start_element("triglav:values");
                
        for (auto j{(*i)->get_list_of_generic_names().begin()}; j != (*i)->get_list_of_generic_names().end(); j++)
        {
            w.start_element("triglav:value");
            
            std::stringstream s;
            (*j)->report(s);
            w.write_comment(s.str());
        
            w.end_element(); // value
        }
        
        
        w.end_element(); // values
        w.end_element(); // type
    }
    
    w.end_element(); // types
    w.end_element(); // origin
    
    w.start_element("triglav:derived");
    std::stringstream s;
    s << a.get_depth();
    w.write_attribute("depth", s.str());
    
    w.start_element("triglav:type_instances");
    for (auto i{a.get_list_of_type_instances().begin()}; i!=a.get_list_of_type_instances().end(); i++)
    {
        w.start_element("triglav:type_instance");
        w.write_attribute("name", (*i)->get_name());
        
        for (auto j{(*i)->get_list_of_value_instances().begin()}; j!=(*i)->get_list_of_value_instances().end(); j++)
        {
            w.start_element("triglav:value_instance");
            w.write_attribute("name", (*j)->get_name());
            w.end_element();
        }
        
        w.end_element(); // type_instance
    }
    w.end_element(); // type_instances
    
    w.start_element("triglav:variable_instances");
    for (auto i{a.get_list_of_variable_instances().begin()}; i!=a.get_list_of_variable_instances().end(); i++)
    {
        w.start_element("triglav:variable_instance");
        w.write_attribute("name", (*i)->get_name());
        w.start_element("triglav:value_instances");
        for (auto j{(*i)->get_vector_of_value_instances().begin()}; j!=(*i)->get_vector_of_value_instances().end(); j++)
        {
            w.start_element("triglav:value_instance");
            w.write_attribute("name", (*j)->get_name());
            
            if (std::find_if((*i)->get_list_of_usual_values().begin(),(*i)->get_list_of_usual_values().end(),[&j](auto x){ return x == (*j)->get_name(); })
                != (*i)->get_list_of_usual_values().end())
            {
                w.write_attribute("usual", "true");
            }
            
            w.end_element(); // value_instance
        }
        w.end_element(); // value_instances
        w.end_element(); // variable_instance
    }
    w.end_element(); // variable_instances
    w.end_element(); // derived
    
    w.end_element(); // specification
    
    
    w.start_element("triglav:case_files");
        
    
    std::stringstream amount_of_case_files_stream;
    
    amount_of_case_files_stream << a.get_amount_of_case_files();
    
    w.write_attribute("amount", amount_of_case_files_stream.str());   
    w.write_attribute("prefix", a.get_case_files_prefix());
    w.write_attribute("extension", a.get_case_files_extension());
    w.write_attribute("path", a.get_case_files_path());
    
    for (auto & i: a.get_vector_of_precalculated_files())
    {
        w.start_element("triglav:precalculated_files");
        
        std::stringstream depth_stream;
        depth_stream << i->get_depth();
        
        w.write_attribute("depth", depth_stream.str());
        w.write_attribute("prefix", i->get_prefix());
        w.write_attribute("done", i->get_done() ? "true" : "false");
        w.end_element();
    }
    w.end_element(); // case files
        
    w.start_element("triglav:generator");
    std::stringstream max_amount_of_unusual_cases_stream;
    max_amount_of_unusual_cases_stream << a.get_max_amount_of_unusual_cases();
    w.write_attribute("max_amount_of_unusual_values", max_amount_of_unusual_cases_stream.str());
    w.end_element(); // generator
    
    w.start_element("triglav:machine");
    std::stringstream amount_of_processors_stream;
    amount_of_processors_stream << a.get_amount_of_processors();    
    w.write_attribute("amount_of_processors", amount_of_processors_stream.str()); 
    w.end_element();        
    
    w.start_element("triglav:validation_range");
    w.end_element(); // validation range

    w.start_element("triglav:visible_states");
    w.write_attribute("prefix", "visible_state_");
    w.write_attribute("extension", "txt");
    w.write_attribute("path", ".");
    w.start_element("triglav:filename_schema");

    unsigned amount_of_input_variables = 0;
    
    for (auto i{a.get_list_of_variable_instances().begin()}; i!=a.get_list_of_variable_instances().end(); i++)
    {
        if ((*i)->get_mode() == variable_mode::INPUT)
        {
            amount_of_input_variables++;
        }
    }
    std::stringstream s3,s4;
    s3 << std::hex << amount_of_input_variables; // it should be amount_of_input_variables-1, but then we would have to handle the case with no input variables, pretty unlikely
    unsigned amount_of_id_digits = s3.str().length();
    
    s4 << amount_of_id_digits;

    w.write_attribute("amount_of_digits", s4.str());
    
    unsigned id = 0;
    
    for (auto i{a.get_list_of_variable_instances().begin()}; i!=a.get_list_of_variable_instances().end(); i++)
    {
        if ((*i)->get_mode() == variable_mode::INPUT)
        {
            w.start_element("triglav:input_variable_instance");            
            w.write_attribute("name", (*i)->get_name());
            
            std::stringstream s5;
            s5 << std::setfill('0') << std::setw(amount_of_id_digits) << std::hex << id++;
            w.write_attribute("id", s5.str());
            
            w.write_attribute("type_instance", (*i)->get_type_instance()->get_name());
            
            w.end_element();
        }
    }
    
    w.end_element(); // filename_schema    
    w.end_element(); // visible_states
    
    w.end_element(); // data
    
    w.end_element(); // databank
#else
    std::cerr << "Please install the libxml2 library (devel version), configure triglav with the option --with-xml, rebuild it and reinstall it.\n";
#endif
}
