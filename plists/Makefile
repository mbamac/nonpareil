VERSION=$(shell cat ../Version)

input = $(wildcard *.plist.in)
output = $(addprefix ../, $(input:%.plist.in=%.plist))

all: $(output)

../%.plist: %.plist.in ../Version
	m4 -DNONPAREIL_VERSION=$(VERSION) $< > $@
