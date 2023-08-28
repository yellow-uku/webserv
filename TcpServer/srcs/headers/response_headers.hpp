#pragma once

#include "all.hpp"

class response_headers{
        public:
                response_headers();
                ~response_headers();
                std::string	http_versus;
                std::string	http_status;
                std::string	connection;
                std::string	content_type;
		std::string	backslash_r_n;
		std::string	headers;
		void		build_headers();
};
