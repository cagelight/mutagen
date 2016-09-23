#pragma once

#include <cstdint>
#include <cstddef>

#include <string>
#include <random>

namespace mutagen {
	
	enum class mutation_type : uint_fast64_t {
		rand_byte,
		invert_bit,
	};
	
	typedef uint8_t gene_t;
	
	struct mutation {
		mutation_type type;
		union {
			struct {
				size_t gene_index;
				gene_t prev_value;
			} m_rand_byte;
			struct {
				size_t gene_index;
				uint_least8_t bit_index;
			} m_invert_bit;
		};
	};
	
	class mutacore {
		
	public:
		
		mutacore(size_t);
		~mutacore();
		
		size_t size() const {return data_size;}
		gene_t const * data() const {return data_ptr;}
		gene_t * data() {return data_ptr;}
		
		void randomize();
		void mutate(mutation_type);
		void mutate(std::vector<mutation_type> const &);
		void undo();
		
		std::string hexadecimal() const;
		std::string quaternary() const;
		std::string binary() const;
		
		mutacore & operator = (mutacore const & other);
		
	private:
		
		void mutate_single(mutation_type, size_t);
		
		gene_t * data_ptr = nullptr;
		size_t data_size = 0;
		
		std::random_device rdev;
		std::mt19937_64 rgen { rdev() };
		
		std::uniform_int_distribution<gene_t> rbyte {0x00, 0xFF};
		std::uniform_int_distribution<size_t> rbit {0, 7};
		std::uniform_int_distribution<size_t> rgene;
		
		std::vector<mutation> last_mut;
	};
	
}
