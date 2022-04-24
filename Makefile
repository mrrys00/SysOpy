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

.PHONY: lab02_tar
lab02_tar:
	mkdir RysSzymon
	mkdir -p archives
	cp -r cw02/ RysSzymon/
	tar czvf archives/RysSzymon-cw02.tar.gz RysSzymon/
	rm -r RysSzymon

.PHONY: lab03_tar
lab03_tar:
	mkdir RysSzymon
	mkdir -p archives
	cp -r cw03/ RysSzymon/
	tar czvf archives/RysSzymon-cw03.tar.gz RysSzymon/
	rm -r RysSzymon

.PHONY: lab04_tar
lab04_tar:
	mkdir RysSzymon
	mkdir -p archives
	cp -r cw04/ RysSzymon/
	tar czvf archives/RysSzymon-cw04.tar.gz RysSzymon/
	rm -r RysSzymon

.PHONY: lab05_tar
lab05_tar:
	mkdir RysSzymon
	mkdir -p archives
	cp -r cw05/ RysSzymon/
	tar czvf archives/RysSzymon-cw05.tar.gz RysSzymon/
	rm -r RysSzymon

.PHONY: lab06_tar
lab06_tar:
	mkdir RysSzymon
	mkdir -p archives
	cp -r cw06/ RysSzymon/
	tar czvf archives/RysSzymon-cw06.tar.gz RysSzymon/
	rm -r RysSzymon
