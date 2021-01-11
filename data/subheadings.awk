BEGIN {
    FS = "\t"
    print ""
    print "kjv_subheading kjv_subheadings[] = {"
}
{
    printf("	{%d, %d, %d, \"%s\"},\n", $1, $2, $3, $4)
}
END {
    print "};"
    print ""
    printf("int kjv_subheadings_length = %d;\n", NR)
}
