#include <assert.h>
#include <pp/pp.h>
#include <stdlib.h>
#include <string.h>



static inline void
s_set_diag_msg(struct as_diag *diag, const char msg[])
{
	assert(strlen(msg) <= AS_MAX_DIAG_MSG_LEN);

	as_set_diag(diag, diag->line, diag->column, msg);
}

static PP_Result
s_run_line(struct pp *pp, const char rd_line[], size_t rd_line_len)
{
	assert(pp && rd_line);
	assert(rd_line_len && rd_line_len <= PP_MAX_LINE_LEN);

	char wr_line[PP_MAX_LINE_LEN + 1];
	size_t wr_line_len = 0;

	size_t i;
	for (i = 0; i < rd_line_len; ++i) {
		switch (pp->state) {
			case PP_STATE_NORMAL:
				switch (rd_line[i]) {
					case '\r':
						break;
					case '\n':
						wr_line[wr_line_len] =
							rd_line[i];
						wr_line_len++;
						break;
					case '\t':
						/* fall through */
					case ' ':
						wr_line[wr_line_len] = ' ';
						wr_line_len++;
						pp->state =
							PP_STATE_WHITE_SPACE;
						break;
					case ';':
						pp->state =
							PP_STATE_LINE_COMMENT;
						break;
					default:
						wr_line[wr_line_len] =
							rd_line[i];
						wr_line_len++;
						break;
				}
				break;
			case PP_STATE_LINE_COMMENT:
				switch (rd_line[i]) {
					case '\r':
						break;
					case '\n':
						wr_line[wr_line_len] =
							rd_line[i];
						wr_line_len++;
						pp->state = PP_STATE_NORMAL;
						break;

					default:
						break;
				}
				break;
			case PP_STATE_WHITE_SPACE:
				switch (rd_line[i]) {
					case '\t':
						/* fall through */
					case ' ':
						break;
					default:
						pp->state = PP_STATE_NORMAL;
						i--;
						break;
				}
				break;
			default:
				assert(0);
				break;
		}
	}

	wr_line[wr_line_len] = '\0';

	return EOF == fputs(wr_line, pp->out) ? PP_RESULT_ERR(PP_ERR_INTERNAL)
					      : PP_RESULT_OK;
}

void
pp_init(OUT_ struct pp *pp, INOUT_ FILE *in, INOUT_ FILE *out, struct as_diag diag)
{
	assert(pp && in && out);

	pp->in = in;
	pp->out = out;
	pp->state = PP_STATE_NORMAL;
}

PP_Result
pp_run(struct pp *pp)
{
	char line[PP_MAX_LINE_LEN + 1] = {
		0,
	};

	while (fgets(line, sizeof(line), pp->in)) {
		const size_t line_len = strlen(line);

		if (!line_len)
			return PP_RESULT_ERR(PP_ERR_INTERNAL);
		/* Every line must be finished with '\n' */
		if (line[line_len - 1] != '\n')
			return PP_RESULT_ERR(PP_ERR_INVAL_LINE_LEN);

		PP_RET_IF_ERR(s_run_line(pp, line, line_len));
	}

	return feof(pp->in) ? PP_RESULT_OK : PP_RESULT_ERR(PP_ERR_INTERNAL);
}
