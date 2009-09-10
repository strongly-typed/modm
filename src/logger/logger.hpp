// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------
#ifndef RCA__LOGGER__HPP
#define RCA__LOGGER__HPP
/**
 * @defgroup logger Logger
 * @brief Log messages to specifyed destiny.
 * 
 * The Logger write messages a destiny specifyed by the used OutputWriter.
 * The Logger is a singelten an act as server to the log messages and 
 * send them to the OutputWriter.\n
 * To log a message use the LoggerMessageForwarder, that will filter the 
 * messages after level an formward them to the Logger.\n
 * 
 * @section howto HowTo
 * - create a logger singelton with a outputWriter
 * - create a loggerFrowarder in the class or methode you want write a message
 * - use the loggerForwarder to write the log message
 * 
 * @section flow Message Flow
 * LoggerMessageForwarder -> Logger -> LoggerOutputWriter -> destiny
 * 
 * @section loglevels Log Levels
 * The following log levels are supported: \n
 * DEBUG < INFO < WARNING < ERROR < FATAL
 * 
 * 
 * @todo write examples
 */

#include <sstream>
#include <ostream>
#include <map>
#include <iomanip>  // std::setw, ::std::setfill 
#include <ios>      // std::left, ::std::right

#include "abstract_output_writer.hpp"
#include "console_output_writer.hpp"
// More to come...


/**
 * @class 	Logger 
 * @brief 	Logger singelton, connect the LoggerMessageForwarder to the 
 * 			LoggerOutputWriter
 * 
 * @todo	Logging of templates is not possible, because the static variable
 * 			of the logger will be also seen in the file, that include the 
 * 			header file.
 * 
 * @todo	HowTo: using of OutputWriter
 *
 * @ingroup logger
 * @version	$Id$
 * @since 	01 December 2006
 * @author	Christofer Hedbrand,
 * 			Carsten Schmidt,
 * 			Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
 */
class Logger {
	public:
		// Define static log priorities here
		typedef enum {
				OFF		=     0,
				DEBUG	=	100,
				INFO	=	200,
				WARNING	=	300,
				ERROR	=	400,
				FATAL	=	500,
		} PriorityLevel;

		static Logger& 
		instance();
		
		void
		setDefaultOutputWriter();

		void 
		setOutputWriter(AbstractOutputWriter& outputWriter);
		
		void 
		setMessageHeader(	const std::string & loggerLocationString,
							const std::string & messageLocationString,
							const Logger::PriorityLevel & messagePriorityLevel);

		
		template <class T> 
		void 
		log(const T& message ) {
			this->outputWriter_->getOutputStream() 	<< message
													<< std::flush;
		};
		
	private:
		Logger();						// Private destructor because of singelton
		Logger(const Logger & logger);	// Private copy constructor because of singelton
		~Logger();
		
		std::string								identifierString_;	
		std::map<PriorityLevel, std::string>	logLevelIdNameMap_;

		AbstractOutputWriter*					outputWriter_;
		AbstractOutputWriter*					defaultOutputWriter_;
};


#endif /* RCA__LOGGER__HPP */
