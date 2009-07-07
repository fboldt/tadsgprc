grep -v '^#include' $1 | 
awk '
	/{/	{ pilha++ }
		{ if (pilha==0) print }
	/}/	{ pilha-- ; if (pilha==0) print ";" };
'

echo -e "\n#endif\n\n"
