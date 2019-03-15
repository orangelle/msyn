#include "bitop.h"

namespace bitprocess {


	int count_or(bitvector& b1, bitvector& b2) {
		if (b1.size() != b2.size())
			return 0;
		int counter = 0;
		for (int i = 0; i < b1.word_size(); i++)
			counter += count64((b1.get_word(i) | b2.get_word(i)));

		return counter;
	}

	int count_and(bitvector& b1, bitvector& b2) {
		if (b1.size() != b2.size())
			return 0;
		int counter = 0;
		for (int i = 0; i < b1.word_size(); i++)
			counter += count64((b1.get_word(i) & b2.get_word(i)));

		return counter;
	}

	int count_or(bitvector& b1, bitvector& b2, bitvector& mask) {
		if (b1.size() != b2.size() || b2.size() != mask.size())
			return 0;
		int counter = 0;
		for (int i = 0; i < b1.word_size(); i++)
			counter += count64((b1.get_word(i) | b2.get_word(i)) & ~mask.get_word(i));

		return counter;
	}

	int count_and(bitvector& b1, bitvector& b2, bitvector& mask) {
		if (b1.size() != b2.size() || b2.size() != mask.size())
			return 0;
		int counter = 0;
		for (int i = 0; i < b1.word_size(); i++)
			counter += count64((b1.get_word(i) & b2.get_word(i)) & ~mask.get_word(i));

		return counter;
	}

}
