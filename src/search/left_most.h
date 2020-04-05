#ifndef LEFT_MOST_H_
#define LEFT_MOST_H_

#include <algorithm>
#include "search.h"
#include "sse_dist.h"
#include "../util/sequence/sequence.h"

namespace Search {

static inline bool left_most_filter(const sequence &query,
	const Letter* subject,
	const int seed_offset,
	const int seed_len,
	const Context &context,
	bool first_shape)
{
	//return true;
	constexpr int WINDOW_LEFT = 16, WINDOW_RIGHT = 32;

	int d = std::max(seed_offset - WINDOW_LEFT, 0), window_left = std::min(WINDOW_LEFT, seed_offset);
	const Letter *q = query.data() + d, *s = subject + d;
	int window = (int)query.length() - d;
	window = std::min(window, window_left + 1 + WINDOW_RIGHT);

	const sequence subject_clipped = Util::Sequence::clip(s, window, window_left);
	window -= s + window - subject_clipped.end();

	d = subject_clipped.data() - s;
	q += d;
	s += d;
	window_left -= d;
	window -= d;

	const uint64_t match_mask = reduced_match(q, s, window);

	const uint32_t len_left = window_left + seed_len - 1,
		match_mask_left = ((1llu << len_left) - 1) & match_mask,
		len_right = window - window_left - 1,
		match_mask_right = match_mask >> (window_left + 1);

	const bool left_hit = context.current_matcher.hit(match_mask_left, len_left);
	if (first_shape)
		return !left_hit;
	
	const bool right_hit = context.previous_matcher.hit(match_mask_right, len_right);
	return !left_hit && !right_hit;
}

}

#endif