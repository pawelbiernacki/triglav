

EXTRA_DIST = *.triglav *.xml *.txt

all:


readme6.xml:
	triglav step1_save_databank.triglav; \
	triglav step2_estimate_cases.triglav > estimated_cases.txt; \
	triglav_databank_set_amount_of_processors.pl readme.xml 2 > readme2.xml; \
	triglav_databank_set_validation_range.pl readme2.xml estimated_cases.txt 0 3 > readme3.xml; \
	triglav_databank_set_amount_of_case_files.pl readme3.xml 4 > readme4.xml; \
	triglav_databank_distribute_workload.pl readme4.xml > readme5.xml; \
	triglav_databank_set_case_files_prefix.pl readme5.xml "cases_" > readme6.xml
	
readme7.xml: readme6.xml
	triglav step3_generate_cases.triglav;	\
	triglav_databank_set_precalculated_files.pl readme6.xml "precalculated_" 1 > readme7.xml
	
readme8.xml: readme7.xml
	triglav step4_precalculate.triglav
	


