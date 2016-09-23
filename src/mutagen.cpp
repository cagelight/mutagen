#include "mutagen.hpp"

#include <cstdlib>
#include <cstring>

static mutagen::gene_t constexpr h4 [] = {
	0b11110000,
	0b00001111
};

static mutagen::gene_t constexpr q2 [] {
	0b11000000,
	0b00110000,
	0b00001100,
	0b00000011
};

static mutagen::gene_t constexpr b1 [] {
	0b10000000,
	0b01000000,
	0b00100000,
	0b00010000,
	0b00001000,
	0b00000100,
	0b00000010,
	0b00000001
};

mutagen::mutacore::mutacore(size_t size) : data_size(size), rgene(0, size - 1) {
	data_ptr = reinterpret_cast<gene_t *>(malloc(size));
}

mutagen::mutacore::~mutacore() {
	if (data_ptr) free(data_ptr);
}

void mutagen::mutacore::randomize() {
	for (size_t i = 0; i < data_size; i++) {
		data_ptr[i] = rbyte(rgen);
	}
}

void mutagen::mutacore::mutate(mutation_type type) {
	
	last_mut.resize(1);
	mutate_single(type, 0);
	
}

void mutagen::mutacore::mutate(std::vector<mutation_type> const & types) {
	
	last_mut.resize(types.size());
	for (size_t i = 0; i < types.size(); i++) {
		mutate_single(types[i], i);
	}
	
}

void mutagen::mutacore::mutate_single(mutation_type type, size_t muti) {
	
	size_t b_i = rgene(rgen);
	gene_t & b = data_ptr[b_i];
	
	switch (type) {
		case mutation_type::rand_byte: {
			last_mut[muti].type = mutation_type::rand_byte;
			last_mut[muti].m_rand_byte.gene_index = b_i;
			last_mut[muti].m_rand_byte.prev_value = b;
			b = rbyte(rgen);
			break;
		}
		case mutation_type::invert_bit: {
			uint_least8_t bit_index = rbit(rgen);
			last_mut[muti].type = mutation_type::invert_bit;
			last_mut[muti].m_invert_bit.gene_index = b_i;
			last_mut[muti].m_invert_bit.bit_index = bit_index;
			b = b ^ b1[bit_index];
			break;
		}
	}
}

void mutagen::mutacore::undo() {
	std::vector<mutation>::reverse_iterator iter {last_mut.rbegin()};
	for (; iter != last_mut.rend(); iter++) {
		switch (iter->type) {
			case mutation_type::rand_byte: {
				data_ptr[iter->m_rand_byte.gene_index] = iter->m_rand_byte.prev_value;
				break;
			}
			case mutation_type::invert_bit: {
				gene_t & b = data_ptr[iter->m_invert_bit.gene_index];
				b = b ^ b1[iter->m_invert_bit.bit_index];
				break;
			}
		}
	}
}

std::string mutagen::mutacore::hexadecimal() const {
	std::string str;
	str.reserve(data_size * 2 + 1);
	for (size_t i = 0; i < data_size; i++) {
		gene_t hi = (data_ptr[i] & h4[0]) >> 4;
		gene_t lo = data_ptr[i] & h4[1];
		str.push_back(hi + (hi > 9 ? 55 : 48));
		str.push_back(lo + (lo > 9 ? 55 : 48));
	}
	return str;
}

#define QUATERNARY_CHAROFFS 81

std::string mutagen::mutacore::quaternary() const {
	std::string str;
	str.reserve(data_size * 4 + 1);
	for (size_t i = 0; i < data_size; i++) {
		str.push_back(((data_ptr[i] & q2[0]) >> 6) + QUATERNARY_CHAROFFS);
		str.push_back(((data_ptr[i] & q2[1]) >> 4) + QUATERNARY_CHAROFFS);
		str.push_back(((data_ptr[i] & q2[2]) >> 2) + QUATERNARY_CHAROFFS);
		str.push_back(((data_ptr[i] & q2[3]) >> 0) + QUATERNARY_CHAROFFS);
	}
	return str;
}

#define BINARY_CHAROFFS 48

std::string mutagen::mutacore::binary() const {
	std::string str;
	str.reserve(data_size * 8 + 1);
	for (size_t i = 0; i < data_size; i++) {
		str.push_back(((data_ptr[i] & b1[0]) >> 7) + BINARY_CHAROFFS);
		str.push_back(((data_ptr[i] & b1[1]) >> 6) + BINARY_CHAROFFS);
		str.push_back(((data_ptr[i] & b1[2]) >> 5) + BINARY_CHAROFFS);
		str.push_back(((data_ptr[i] & b1[3]) >> 4) + BINARY_CHAROFFS);
		str.push_back(((data_ptr[i] & b1[4]) >> 3) + BINARY_CHAROFFS);      
		str.push_back(((data_ptr[i] & b1[5]) >> 2) + BINARY_CHAROFFS);
		str.push_back(((data_ptr[i] & b1[6]) >> 1) + BINARY_CHAROFFS);
		str.push_back(((data_ptr[i] & b1[7]) >> 0) + BINARY_CHAROFFS);
	}
	return str;
}

mutagen::mutacore & mutagen::mutacore::operator = (mutacore const & other) {
	this->data_size = other.data_size;
	data_ptr = reinterpret_cast<gene_t *>(realloc(data_ptr, data_size));
	memcpy(data_ptr, other.data_ptr, data_size);
	return *this;
}
