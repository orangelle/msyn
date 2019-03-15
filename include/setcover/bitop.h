#ifndef BITOP_H_INCLUDED
#define BITOP_H_INCLUDED

#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <vector>

namespace bitprocess {

	//inline int count64(uint64_t x);
  const uint64_t m1 = 0x5555555555555555;
  const uint64_t m2 = 0x3333333333333333;
  const uint64_t m4 = 0x0f0f0f0f0f0f0f0f;
  const uint64_t m8 = 0x00ff00ff00ff00ff;
  const uint64_t m16 = 0x0000ffff0000ffff;
  const uint64_t m32 = 0x00000000ffffffff;
  const uint64_t hff = 0xffffffffffffffff;
  const uint64_t h01 = 0x0101010101010101;

  inline int count64(uint64_t x) {
    x -= (x >> 1) & m1;
    x = (x & m2) + ((x >> 2) & m2);
    x = (x + (x >> 4)) & m4;
    return (x * h01) >> 56;
  }

	class bitvector {
	private:
		unsigned int bsize;
		unsigned int wsize;
		unsigned int counter;
		std::vector<uint64_t> data;
		static const int WSIZE = 64;

	public:
		bitvector(int size) {
			set_size(size);
		}

		bitvector() {
			bsize = 0;
			wsize = 0;
			counter = 0;
		}

		~bitvector() { bsize = 0; wsize = 0; data.clear(); }

	public:
		void copy(bitvector &cpy) {
			bsize = cpy.size();
			wsize = cpy.word_size();
			counter = cpy.count();
			data.resize(wsize);
			for (int i = 0; i < wsize; i++)
				data[i] = cpy.get_word(i);
		}

		void clear() {
			counter = 0;
			for (int i = 0; i < wsize; i++)
				data[i] = 0ULL;
		}

		void setall() {
			counter = bsize;
			int i = 0;
			while (i < wsize - 1)
				data[i++] = 0xFFFFFFFFFFFFFFFFULL;
			int p = bsize%WSIZE;
			if (!p) {
				data[i] = 0xFFFFFFFFFFFFFFFFULL;
			}
			else {
				uint64_t d = 0ULL;
				while (p > 0) {
					d = d << 1;
					d |= 1ULL;
					p--;
				}
				data[i] = d;
			}
		}

		void resize(int newsize) {
			if (newsize == bsize)
				return;

			int nbsize = newsize;
			int nwsize = nbsize / WSIZE; if (nbsize%WSIZE != 0) nwsize++;

			//if (nbsize > bsize) only this case happen
			if (nwsize > wsize) {
				data.resize(nwsize);
				for (int i = wsize; i < nwsize; i++)
					data[i] = 0ULL;
				wsize = nwsize;
			}
			bsize = nbsize;
		}

		void set_size(int size) {
			bsize = size;
			wsize = bsize / WSIZE; if (bsize%WSIZE != 0) wsize++;
			counter = 0;
			data.resize(wsize);
			for (int i = 0; i < wsize; i++)
				data[i] = 0ULL;
		}

		int size() { return bsize; }

		unsigned int count() { return counter; }

		void set(int pos) {
			int p1 = pos / WSIZE; int p2 = pos%WSIZE;
			uint64_t mask = 1ULL << p2;
			uint64_t bval = data[p1] & mask;
			if (!bval) {  //If this position is ready "1", then don't need to do anything
				data[p1] |= mask;
				counter++;
			}
		}

		void reset(int pos) {
			int p1 = pos / WSIZE; int p2 = pos%WSIZE;
			uint64_t mask = 1ULL << p2;
			uint64_t bval = data[p1] & mask;
			if (bval) {
				data[p1] &= (~mask);
				counter--;
			}
		}

		bool is_set(int pos) {
			int p1 = pos / WSIZE; int p2 = pos%WSIZE;
			uint64_t mask = 1ULL << p2;
			uint64_t bval = data[p1] & mask;
			return (bval != 0ULL);
		}

		bool is_allset() {
			return (count() == bsize);
		}

		//find "1" position from pos+1
		int next_set(int pos) {
			pos++;
			if (pos >= bsize) return bsize;
			int p1 = pos / WSIZE; int p2 = pos%WSIZE;

			uint64_t d = data[p1] >> p2;
			if (d != 0ULL) {
				while (p2 < WSIZE && pos < bsize) {
					if (d & 1ULL) return pos;
					pos++;
					p2++;
					d = d >> 1;
				}
				if (pos >= bsize)
					return bsize;
			}
			//if this words don't have "1", then start from next words until end
			p1++;
			pos = p1*WSIZE;
			while (p1 < wsize - 1) {
				p2 = 0;
				d = data[p1];
				if (d != 0ULL) {
					while (p2 < WSIZE) {
						if (d & 1ULL) return pos;
						pos++;
						p2++;
						d = d >> 1;
					}
				}
				p1++;
				pos = p1*WSIZE;
			}
			//if no "1" can be found from position, then find from the beginning of this word
			p2 = 0;
			d = data[p1];
			if (d != 0ULL) {
				while (p2 < WSIZE && pos < bsize) {
					if (d & 1ULL) return pos;
					pos++;
					p2++;
					d = d >> 1;
				}
			}

			return bsize;
		}

		unsigned int recount() {
			counter = 0;
			for (int i = 0; i < wsize; i++) {
				switch (data[i]) {
				case 0x0ULL:
					break;
				case 0xFFFFFFFF00000000ULL:
				case 0x00000000FFFFFFFFULL:
					counter += 32;
					break;
				case 0xFFFFFFFFFFFFFFFFULL:
					counter += 64;
					break;
				default:
					counter += count64(data[i]);
				}
			}
			return counter;
		}

		int word_size() { return wsize; }

		uint64_t get_word(int loc) { return data[loc]; }

		int or_count(bitvector& b) {
			for (int i = 0; i < wsize; i++)
				data[i] |= b.get_word(i);

			return recount();
		}

		int and_count(bitvector& b) {
			//if (b.size() != bsize)
			//  return 0;
			for (int i = 0; i < wsize; i++)
				data[i] &= b.get_word(i);

			return recount();
		}

		int mask_count(bitvector& b) {
			//if (b.size() != bsize)
			//  return 0;
			for (int i = 0; i < wsize; i++)
				data[i] &= (~b.get_word(i));

			return recount();
		}

		int not_count() {
			for (int i = 0; i < wsize; i++)
				data[i] = ~data[i];

			counter = bsize - counter;
			return counter;
		}
	};

	int count_or(bitvector& b1, bitvector& b2);
	int count_and(bitvector& b1, bitvector& b2);
	int count_or(bitvector& b1, bitvector& b2, bitvector& mask);
	int count_and(bitvector& b1, bitvector& b2, bitvector& mask);


}


#endif
