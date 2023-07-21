# 顶层Makefile设计 
.PHONY:all
all:
	@# @表示不显示执行过程，\表示续行，这里是一整个语句
	@cd CompileServer;\
	make;\
	cd -;\
	cd OJServer;\
	make;\
	cd -;

# 项目发布
.PHONY:output
output:
	@mkdir -p output/CompileServer;\
	mkdir -p output/OJServer;\
	cp -rf CompileServer/compile_server output/CompileServer;\
	cp -rf OJServer/oj_server output/OJServer;\
	cp -rf CompileServer/temp output/CompileServer;\
	cp -rf OJServer/conf output/OJServer;\
	cp -rf OJServer/lib output/OJServer;\
	cp -rf OJServer/questions output/OJServer;\
	cp -rf OJServer/template_html output/OJServer;\
	cp -rf OJServer/wwwroot output/OJServer;

.PHONY:clean
clean:
	@cd CompileServer;\
	make clean;\
	cd -;\
	cd OJServer;\
	make clean;\
	cd -;\
	rm -rf output;
