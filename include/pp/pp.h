#ifndef WA824I_PP_PP_H_
#define WA824I_PP_PP_H_

#include <stdio.h>
#include <pp/result.h>
#include <pp/def.h>
#include <mac.h>
#include <as/err.h>

enum PP_STATE_ {
	PP_STATE_NORMAL,
	PP_STATE_LINE_COMMENT,
	PP_STATE_WHITE_SPACE,
};

struct pp {
	FILE *in;
	FILE *out;
	enum PP_STATE_ state;
};


void
pp_init(OUT_ struct pp *pp, INOUT_ FILE *in, INOUT_ FILE *out, struct as_diag diag);

PP_Result
pp_run(INOUT_ struct pp *pp);

void
pp_deinit(ALLOC_ TAKE_ struct pp *pp);




#endif
