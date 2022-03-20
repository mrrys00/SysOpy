.PHONY: submodules
submodules:
	git submodule update --init --recursive --remote

.PHONY: lab01_tar
lab01_tar:
	mkdir RysSzymon
	mkdir -p archives
	cp -r cw01/ RysSzymon/
	tar czvf archives/RysSzymon-cw01.tar.gz RysSzymon/
	rm -r RysSzymon
