/* $Id: pstream.h,v 1.17.4.2 2002/08/27 00:20:45 redi Exp $
PStreams - POSIX Process I/O for C++
Copyright (C) 2001-2002 Jonathan Wakely

This file is part of PStreams.

PStreams is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 2.1 of
the License, or (at your option) any later version.

PStreams is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with PStreams; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
 * @file pstream.h
 * @brief Declares all PStreams classes.
 * @author Jonathan Wakely
 *
 * Defines classes redi::ipstream, redi::opstream, and, conditionally,
 * redi::pstream.
 * Also defines utility function redi::openmode2str() to convert a
 * @c std::ios_base::openmode to a @c std::string.
 */

#ifndef REDI_PSTREAM_H
#define REDI_PSTREAM_H

/// The library version.
#define PSTREAMS_VERSION 0x0011   // 0.17

// check whether to provide pstream
// popen() needs to use bidirectional pipe
#if ! defined(REDI_PSTREAMS_POPEN_USES_BIDIRECTIONAL_PIPE)
#if defined(__FreeBSD__) && __FreeBSD__ >= 3
# define REDI_PSTREAMS_POPEN_USES_BIDIRECTIONAL_PIPE 1
#elif defined(__NetBSD_Version__) && __NetBSD_Version__ >= 0x01040000
# define REDI_PSTREAMS_POPEN_USES_BIDIRECTIONAL_PIPE 1
#endif
#endif


#include <ios>
#include <streambuf>
#include <istream>
#include <ostream>
#include <string>
#include <cstdio>
#include <cerrno>

// TODO add buffering to pstreambuf

// TODO replace popen() with handrolled version


/// All PStreams classes are declared in namespace redi.
namespace redi
{
  /// Convert an ios_base::openmode to a string.
  std::string
  openmode2str(std::ios_base::openmode mode);
  // TODO extend openmode2str() to handle all openmode values

  /// Class template for stream buffer.
  template <typename CharT, typename Traits>
    class basic_pstreambuf : public std::basic_streambuf<CharT, Traits>
    {
    public:
      // Type definitions for dependent types
      typedef CharT                             char_type;
      typedef Traits                            traits_type;
      typedef typename traits_type::int_type    int_type;
      typedef typename traits_type::off_type    off_type;
      typedef typename traits_type::pos_type    pos_type;

      /// Default constructor.
      basic_pstreambuf();

      /// Constructor that initialises the buffer.
      basic_pstreambuf(const std::string& command, std::ios_base::openmode mode);

      /// Destructor.
      ~basic_pstreambuf();

      /// Initialise the stream buffer.
      basic_pstreambuf*
      open(const std::string& command, std::ios_base::openmode mode);

      /// Close the stream buffer.
      basic_pstreambuf*
      close();

      /// Report whether the stream buffer has been initialised.
      bool
      is_open() const;

    protected:
      /// Transfer characters to the pipe when character buffer overflows.
      int_type
      overflow(int_type c);

      /// Transfer characters from the pipe when the character buffer is empty.
      int_type
      uflow();

      /// Transfer characters from the pipe when the character buffer is empty.
      int_type
      underflow();

      /// Make a character available to be returned by the next extraction.
      int_type
      pbackfail(int_type c = traits_type::eof());

      std::streamsize
      write(char_type* s, std::streamsize n);

      std::streamsize
      read(char_type* s, std::streamsize n);

      /// Insert a character into the pipe.
      bool
      write(char_type c);

      /// Extract a character from the pipe.
      bool
      read(char_type& c);

    private:
      basic_pstreambuf(const basic_pstreambuf&);
      basic_pstreambuf& operator=(const basic_pstreambuf&);

      FILE*         file_;
      char_type     char_buf_;
      bool          take_from_buf_;
    };


  /// Class template for Input PStreams.
  template <typename CharT, typename Traits = std::char_traits<CharT> >
    class basic_ipstream : public std::basic_istream<CharT, Traits>
    {
      typedef std::basic_istream<CharT, Traits>     istream_type;
      typedef basic_pstreambuf<CharT, Traits>       streambuf_type;

    public:
      /// Default constructor.
      basic_ipstream();

      /// Constructor that initialises the stream by starting a process.
      basic_ipstream(const std::string& command, std::ios_base::openmode mode = ios_base::in);

      /// Destructor
      ~basic_ipstream() { }

      /// Start a process.
      void
      open(const std::string& command, std::ios_base::openmode mode = std::ios_base::in);

      /// Close the pipe.
      void
      close();

      /// Report whether the stream's buffer has been initialised.
      bool
      is_open() const;

      /// Return the command used to initialise the stream.
      const std::string&
      command() const;

    private:
      std::string       command_;
      streambuf_type    buf_;
    };


  /// Class template for Output PStreams.
  template <typename CharT, typename Traits = std::char_traits<CharT> >
    class basic_opstream : public std::basic_ostream<CharT, Traits>
    {
      typedef std::basic_ostream<CharT, Traits>     ostream_type;
      typedef basic_pstreambuf<CharT, Traits>       streambuf_type;

    public:
      /// Default constructor.
      basic_opstream();

      /// Constructor that initialises the stream by starting a process.
      basic_opstream(const std::string& command, std::ios_base::openmode mode = std::ios_base::out);

      /// Destructor
      ~basic_opstream() { }

      /// Start a process.
      void
      open(const std::string& command, std::ios_base::openmode mode = std::ios_base::out);

      /// Close the pipe.
      void
      close();

      /// Report whether the stream's buffer has been initialised.
      bool
      is_open() const;

      /// Return the command used to initialise the stream.
      const std::string&
      command() const;

    private:
      std::string       command_;
      streambuf_type    buf_;
    };

  /// Type definition for common template specialisation.
  typedef basic_ipstream<char> ipstream;
  /// Type definition for common template specialisation.
  typedef basic_opstream<char> opstream;


  // member definitions for pstreambuf

  /**
   * @class basic_pstreambuf
   * Provides underlying streambuf functionality for the PStreams classes.
   */

  /** Creates an uninitialised stream buffer. */
  template <typename C, typename T>
    inline
    basic_pstreambuf<C,T>::basic_pstreambuf()
    : file_(0)
    , take_from_buf_(false)
    { }

  /**
   * Initialises the stream buffer by calling open() with the supplied
   * arguments.
   *
   * @param command a string containing a shell command.
   * @param mode the I/O mode to use when opening the pipe.
   * @see open()
   */
  template <typename C, typename T>
    inline
    basic_pstreambuf<C,T>::basic_pstreambuf( const std::string& command, std::ios_base::openmode mode )
    : file_(0)
    , take_from_buf_(false)
    {
      this->open(command, mode);
    }

  /**
   * Closes the stream by calling close().
   * @see close()
   */
  template <typename C, typename T>
    inline
    basic_pstreambuf<C,T>::~basic_pstreambuf()
    {
      this->close();
    }

  /**
   * Starts a new process by passing @a command to the shell
   * and opens a pipe to the process with the specified @a mode.
   * There is no way to tell whether the shell command succeeded, this
   * function will always succeed unless resource limits (such as
   * memory usage, or number of processes or open files) are exceeded.
   *
   * @param command a string containing a shell command.
   * @param mode the I/O mode to use when opening the pipe.
   * @return NULL if the shell could not be started or the
   * pipe could not be opened, pointer to self otherwise.
   */
  template <typename C, typename T>
    inline basic_pstreambuf<C,T>*
    basic_pstreambuf<C,T>::open( const std::string& command, std::ios_base::openmode mode )
    {
      basic_pstreambuf<C,T>* ret = NULL;
      if (!this->is_open())
      {
          file_ = ::popen(command.c_str(), openmode2str(mode).c_str());
          if (this->is_open())
              ret = this;
      }
      return ret;
    }

  /**
   * Waits for the associated process to finish and closes the pipe.
   * @return pointer to self or @c NULL if @c pclose() fails.
   */
  template <typename C, typename T>
    inline basic_pstreambuf<C,T>*
    basic_pstreambuf<C,T>::close()
    {
      basic_pstreambuf<C,T>* ret = NULL;
      if (this->is_open())
      {
        errno = 0;
        ::pclose(file_);
        if (errno==0)
        {
          ret = this;
          file_ = 0;
        }
      }
      return ret;
    }

  /**
   * @return true if open() has been successfully called, false otherwise.
   * @warning Can't determine whether the command used to
   * initialise the buffer was successfully executed or not. If the
   * shell command failed this function will still return true.
   * @see open()
   */
  template <typename C, typename T>
    inline bool
    basic_pstreambuf<C,T>::is_open() const
    {
      return bool(file_);
    }


  /**
   * Called when the internal character buffer is not present or is full,
   * to transfer the buffer contents to the pipe. For unbuffered streams
   * this is called for every insertion.
   *
   * @param c a character to be written to the pipe
   * @return @c traits_type::not_eof(c) if @a c is equal to @c
   * traits_type::eof(). Otherwise returns @a c if @a c can be written
   * to the pipe, or @c traits_type::eof() if not.
   */
  template <typename C, typename T>
    typename basic_pstreambuf<C,T>::int_type
    basic_pstreambuf<C,T>::overflow(int_type c)
    {
      if (!traits_type::eq_int_type(c, traits_type::eof()))
      {
        if (!this->write(traits_type::to_char_type(c)))
          return traits_type::eof();
        else
          return c;
      }
      return traits_type::not_eof(c);
    }

  /**
   * Called when the internal character buffer is not present or is empty,
   * to re-fill the buffer from the pipe. Behaves like underflow() but also
   * increments the next pointer to the get area, so repeated calls will
   * return a different character each time.
   *
   * @return The next available character in the buffer,
   * or @c traits_type::eof() in case of failure.
   * @see underflow()
   */
  template <typename C, typename T>
    typename basic_pstreambuf<C,T>::int_type
    basic_pstreambuf<C,T>::uflow()
    {
      if (take_from_buf_)
      {
        take_from_buf_ = false;
        return traits_type::to_int_type(char_buf_);
      }
      else
      {
        char_type c;

        if (!this->read(c))
        {
          return traits_type::eof();
        }
        else
        {
          char_buf_ = c;
          return traits_type::to_int_type(c);
        }
      }
    }

  /**
   * Called when the internal character buffer is not present or is empty,
   * to re-fill the buffer from the pipe. For unbuffered streams this is
   * called for every extraction.
   *
   * @return The first available character in the buffer,
   * or @c traits_type::eof() in case of failure.
   * @see uflow()
   */
  template <typename C, typename T>
    typename basic_pstreambuf<C,T>::int_type
    basic_pstreambuf<C,T>::underflow()
    {
      if (take_from_buf_)
      {
        return traits_type::to_int_type(char_buf_);
      }
      else
      {
        char_type c;

        if (!this->read(c))
        {
          return traits_type::eof();
        }
        else
        {
          take_from_buf_ = true;
          char_buf_ = c;
          return traits_type::to_int_type(c);
        }
      }
    }

  /**
   * Attempts to make @a c available as the next character to be read by
   * @c this->sgetc(), or if @a c is equal to @c traits_type::eof() then
   * the previous character in the sequence is made the next available ???
   *
   * @param c a character to make available for extraction.
   * @return @a c if the character can be made available, @c traits_type::eof()
   * otherwise.
   */
  template <typename C, typename T>
    typename basic_pstreambuf<C,T>::int_type
    basic_pstreambuf<C,T>::pbackfail(int_type c)
    {
      if (!take_from_buf_)
      {
        if (!traits_type::eq_int_type(c, traits_type::eof()))
        {
          char_buf_ = traits_type::to_char_type(c); 
        }
        take_from_buf_ = true;
        return traits_type::to_int_type(char_buf_);
      }
      else
      {
         return traits_type::eof();
      }
    }

  /**
   * Attempts to insert @a c into the pipe. Used by overflow().
   *
   * @param c a character to insert.
   * @return true if the character could be inserted, false otherwise.
   */
  template <typename C, typename T>
    inline bool
    basic_pstreambuf<C,T>::write(char_type c)
    {
      return (this->write(&c, 1) == 1);
    }

  /**
   * Attempts to extract a character from the pipe and store it in @a c.
   * Used by underflow().
   *
   * @param c a reference to hold the extracted character.
   * @return true if a character could be extracted, false otherwise.
   */
  template <typename C, typename T>
    inline bool
    basic_pstreambuf<C,T>::read(char_type& c)
    {
      return (this->read(&c, 1) == 1);
    }

  /**
   * Writes up to @a n characters to the pipe from the buffer @a s.
   *
   * @param s character buffer.
   * @param n buffer length.
   * @return the number of characters written.
   */
  template <typename C, typename T>
    inline std::streamsize
    basic_pstreambuf<C,T>::write(char_type* s, std::streamsize n)
    {
      return (file_ ? std::fwrite(s, sizeof(char_type), n, file_) : 0);
    }

  /**
   * Reads up to @a n characters from the pipe to the buffer @a s.
   *
   * @param s character buffer.
   * @param n buffer length.
   * @return the number of characters read.
   */
  template <typename C, typename T>
    inline std::streamsize
    basic_pstreambuf<C,T>::read(char_type* s, std::streamsize n)
    {
      return (file_ ? std::fread(s, sizeof(char_type), n, file_) : 0);
    }


  // member definitions for basic_ipstream

  /**
   * @class basic_ipstream
   * Reading from an ipstream reads the command's standard output,
   * and the command's standard input is the same as that of the process
   * that created the object.
   */

  /** Creates an uninitialised stream. */
  template <typename C, typename T>
    inline
    basic_ipstream<C,T>::basic_ipstream()
    : istream_type(NULL)
    , command_()
    , buf_()
    {
      this->init(&buf_);
    }
    
  /**
   * Initialises the stream buffer by calling open() with the supplied
   * arguments.
   *
   * @param command a string containing a shell command.
   * @param mode the I/O mode to use when opening the pipe.
   * @see open()
   */
  template <typename C, typename T>
    inline
    basic_ipstream<C,T>::basic_ipstream( const std::string& command, std::ios_base::openmode mode )
    : istream_type(NULL)
    , command_(command)
    , buf_()
    {
      this->init(&buf_);
      this->open(command_, mode);
    }

  /**
   * Starts a new process by passing <em>command</em> to the shell
   * and opens a pipe to the process with the specified mode.
   *
   * @param command a string containing a shell command.
   * @param mode the I/O mode to use when opening the pipe.
   * @see basic_pstreambuf::open()
   */
  template <typename C, typename T>
    inline void
    basic_ipstream<C,T>::open( const std::string& command, std::ios_base::openmode mode )
    {
      if (!buf_.open((command_=command), (mode|std::ios_base::in)))
        this->setstate(std::ios_base::failbit);
    }

  /** Waits for the associated process to finish and closes the pipe. */
  template <typename C, typename T>
    inline void
    basic_ipstream<C,T>::close()
    {
      if (!buf_.close())
        this->setstate(std::ios_base::failbit);
    }

  /**
   * @return true if open() has been successfully called, false otherwise.
   * @see basic_pstreambuf::open()
   */
  template <typename C, typename T>
    inline bool
    basic_ipstream<C,T>::is_open() const
    {
      return buf_.is_open();
    }

  /** @return a string containing the command used to initialise the stream. */
  template <typename C, typename T>
    inline const std::string&
    basic_ipstream<C,T>::command() const
    {
      return command_;
    }


  // member definitions for basic_opstream

  /**
   * @class basic_opstream
   * 
   * Writing to an open opstream writes to the standard input of the command;
   * the command's standard output is the same as that of the process that
   * created the pstream object, unless this is altered by the command itself.
   */

  /** Creates an uninitialised stream. */
  template <typename C, typename T>
    inline
    basic_opstream<C,T>::basic_opstream()
    : ostream_type(NULL)
    , command_()
    , buf_()
    {
      this->init(&buf_);
    }

  /**
   * Initialises the stream buffer by calling open() with the supplied
   * arguments.
   *
   * @param command a string containing a shell command.
   * @param mode the I/O mode to use when opening the pipe.
   * @see open()
   */
  template <typename C, typename T>
    inline
    basic_opstream<C,T>::basic_opstream( const std::string& command, std::ios_base::openmode mode )
    : ostream_type(NULL)
    , command_(command)
    , buf_()
    {
      this->init(&buf_);
      this->open(command_, mode);
    }

  /**
   * Starts a new process by passing @a command to the shell
   * and opens a pipe to the process with the specified @a mode.
   *
   * @param command a string containing a shell command.
   * @param mode the I/O mode to use when opening the pipe.
   * @see basic_pstreambuf::open()
   */
  template <typename C, typename T>
    inline void
    basic_opstream<C,T>::open( const std::string& command, std::ios_base::openmode mode )
    {
      if (!buf_.open((command_=command), (mode|std::ios_base::out)))
        this->setstate(std::ios_base::failbit);
    }

  /** Waits for the associated process to finish and closes the pipe. */
  template <typename C, typename T>
    inline void
    basic_opstream<C,T>::close()
    {
      if (!buf_.close())
        this->setstate(std::ios_base::failbit);
    }

  /**
   * @see basic_pstreambuf::open()
   * @return true if open() has been successfully called, false otherwise.
   */
  template <typename C, typename T>
    inline bool
    basic_opstream<C,T>::is_open() const
    {
      return buf_.is_open();
    }

  /** @return a string containing the command used to initialise the stream. */
  template <typename C, typename T>
    inline const std::string&
    basic_opstream<C,T>::command() const
    {
      return command_;
    }


  // non-member function definitions

  /**
   * @a mode is masked with @c (ios_base::in|ios_base::out)
   * and converted to a string containing a stdio-style mode.
   *
   * @param mode an I/O mode to convert.
   * @return one of "r", "w", "r+" or "" as a string.
   */
  inline std::string
  openmode2str( std::ios_base::openmode mode )
  {
    using std::ios_base;
    // restrict to the openmodes that pstreambuf honours
    mode &= (ios_base::in|ios_base::out);
    if (mode == ios_base::in)
      return "r";
    else if (mode == ios_base::out)
      return "w";
    else if (mode == (ios_base::in|ios_base::out))
      return "r+";
    else
      return "";
  }

#if REDI_PSTREAMS_POPEN_USES_BIDIRECTIONAL_PIPE

  /// Class template for Bidirectional PStreams.
  template <typename CharT, typename Traits = std::char_traits<CharT> >
    class basic_pstream : public std::basic_iostream<CharT, Traits>
    {
      typedef std::basic_iostream<CharT, Traits>    iostream_type;
      typedef basic_pstreambuf<CharT, Traits>       streambuf_type;

    public:
      /// Default constructor.
      basic_pstream();

      /// Constructor that initialises the stream by starting a process.
      basic_pstream(const std::string& command, std::ios_base::openmode mode = std::ios_base::in|std::ios_base::out);

      /// Destructor
      ~basic_pstream() { }

      /// Start a process.
      void
      open(const std::string& command, std::ios_base::openmode mode = std::ios_base::in|std::ios_base::out);

      /// Close the pipe.
      void
      close();

      /// Report whether the stream's buffer has been initialised.
      bool
      is_open() const;

      /// Return the command used to initialise the stream.
      const std::string&
      command() const;

    private:
      std::string command_;
      streambuf_type buf_;
    };

  /// Type definition for common template specialisation.
  typedef basic_pstream<char> pstream;


  // member definitions for basic_pstream

  /**
   * @class basic_pstream
   * The pstream class for both reading and writing to a process is only
   * available on some sytems, where popen(3) is implemented using a
   * bidirectional pipe.
   */

  /**
   * Creates an uninitialised stream.
   */
  template <typename C, typename T>
    basic_pstream<C,T>::basic_pstream()
    : iostream_type(NULL)
    , command_()
    , buf_()
    {
      this->init(&buf_);
    }

  /**
   * Initialises the stream buffer by calling open() with the supplied
   * arguments.
   * @param command a string containing a shell command.
   * @param mode the I/O mode to use when opening the pipe.
   * @see open()
   */
  template <typename C, typename T>
    basic_pstream<C,T>::basic_pstream(const std::string& command, std::ios_base::openmode mode)
    : iostream_type(NULL)
    , command_(command)
    , buf_()
    {
      this->init(&buf_);
      this->open(command_, mode);
    }

  /**
   * Starts a new process by passing @a command to the shell
   * and opens a pipe to the process with the specified @a mode.
   * @param command a string containing a shell command.
   * @param mode the I/O mode to use when opening the pipe.
   * @see basic_pstreambuf::open()
   */
  template <typename C, typename T>
    inline void
    basic_pstream<C,T>::open(const std::string& command, std::ios_base::openmode mode)
    {
      if (!buf_.open((command_=command), mode))
        this->setstate(std::ios_base::failbit);
    }

  /** Waits for the associated process to finish and closes the pipe. */
  template <typename C, typename T>
    inline void
    basic_pstream<C,T>::close()
    {
      if (!buf_.close())
        this->setstate(std::ios_base::failbit);
    }

  /**
   * @see basic_pstreambuf::open()
   * @return true if open() has been successfully called, false otherwise.
   */
  template <typename C, typename T>
    inline bool
    basic_pstream<C,T>::is_open() const
    {
      return buf_.is_open();
    }

  /** @return a string containing the command used to initialise the stream. */
  template <typename C, typename T>
    inline const std::string&
    basic_pstream<C,T>::command() const
    {
      return command_;
    }


#endif

} // namespace redi

/*
 * rest of file contains additional doxygen comments...
 */

/**
 * @def PSTREAMS_VERSION
 * The value is @c ((major*0xff)+minor), where @c major is the number to
 * the left of the decimal point and @c minor is the number to the right.
 */

/**
 * @mainpage
 * @htmlinclude pstreams.html
 */
// TODO don't use @htmlinclude here, 


#endif  // REDI_PSTREAM_H

