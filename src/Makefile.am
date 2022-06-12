
EXTRA_DIST = parser.y *.txt *.sh *.expected *.triglav

bin_PROGRAMS = triglav

lib_LTLIBRARIES = libtriglav.la

libtriglav_la_SOURCES = scanner.l agent.cc type.cc generic_name_list.cc generic_name_item_token.cc generic_name_item_placeholder.cc type_expression_type_name.cc variable.cc \
		type_instance.cc variable_instance.cc expression_variable_initial_value.cc expression_variable_terminal_value.cc \
		expression_value.cc rule.cc belief.cc my_iterator.cc statement_assert_is_true.cc statement_illegal.cc input.cc statement_assert_is_false.cc \
		logical_expression_equality.cc cpp_parser.cc command_expand.cc command_loop.cc command_report.cc output.cc logical_expression_and.cc \
		statement_add_payoff.cc statement_there_is_a_chance.cc input_variable.cc output_variable.cc hidden_variable.cc input_variable_instance.cc \
		output_variable_instance.cc hidden_variable_instance.cc set_of_possible_values.cc command_generate_cases.cc command_validate_cases.cc \
		cases_parser.cc command_save_databank.cc command_open_databank.cc statement_impossible.cc command_estimate_cases.cc command_consider.cc \
		my_iterator_for_estimating_variable_instances.cc my_iterator_for_variable_instances.cc \
		my_single_range_iterator_for_variable_instances.cc

triglav_SOURCES = triglav.cc 

triglav_LDADD = libtriglav.la @XML_LIBS@

AM_CXXFLAGS = @CXXFLAGS@ @XML_CFLAGS@ -fconcepts -std=c++17 -I.. -I../inc 

parser.tab.h: parser.y
	bison -d $^

clean-local:
	rm -f parser.tab.h
		
lex.triglav.c: scanner.l
	flex $^
	
scanner.c: lex.triglav.c
	cp $^ $@

scanner.lo: parser.tab.h
	

TESTS = test1.sh test2.sh test3.sh xtest1.sh xtest2.sh xtest3.sh

XFAIL_TESTS = xtest1.sh xtest2.sh xtest3.sh

