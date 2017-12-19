/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#ifndef IVW_IVFSEQUENCEVOLUMEWRITER_H
#define IVW_IVFSEQUENCEVOLUMEWRITER_H

#include <modules/base/basemoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <modules/base/io/ivfvolumewriter.h>

namespace inviwo {
/**
 * \ingroup dataio
 * \brief writter for *.ivfs files
 *
 * Supports writing a volume sequence to disk. Will create one main file ([name].ivfs) and a series
 * of ivf volumes ([name]xx.ivf), one for each element in the sequence.
 */
class IVW_MODULE_BASE_API IvfSequenceVolumeWriter
/*: public DataWriterType<VolumeSequence> */  // deriving from DataWriterType does not yet work for
                                              // types not extending data
{
public:
    IvfSequenceVolumeWriter();
    IvfSequenceVolumeWriter(const IvfSequenceVolumeWriter& rhs) = default;
    IvfSequenceVolumeWriter& operator=(const IvfSequenceVolumeWriter& that) = default;
    virtual IvfSequenceVolumeWriter* clone() const { return new IvfSequenceVolumeWriter(*this); }
    virtual ~IvfSequenceVolumeWriter() = default;

    /**
     * \brief Writes a volume sequence to disk
     *
     * Writes a volume sequence to disk. Will create one main file ([name].ivfs) and a series of
     * ivf volumes ([name]xx.ivf), one for each element in the sequence.
     *
     * @param volumes The volume sequence to export
     * @param filePath path to where the files will be written
     */
    virtual void writeData(const VolumeSequence* volumes, const std::string filePath) const;

    /**
     * \brief Writes a volume sequence to disk
     *
     * Writes a volume sequence to disk. Will create one main file ([name].ivfs) and a series of
     * ivf volumes ([name]xx.ivf), one for each element in the sequence.
     *
     * @param volumes The volume sequence to export
     * @param name the name of the dataset, will be used for to name the output files [name].ivfs
     * and [name]xx.ivf
     * @param path path to the folder to put the main file
     * @param reltivePathToElements relative path (from the path to the main file) to where the
     * sequence elements will be written.
     */
    void writeData(const VolumeSequence* volumes, std::string name, std::string path,
                   std::string reltivePathToTimesteps = "") const;

    void setOverwrite(bool overwrite) { overwrite_ = overwrite; }
    bool getOverwrite() const { return overwrite_; }

private:
    IvfVolumeWriter writer_;
    bool overwrite_;
};

namespace util {
/**
 * \brief Writes a volume sequence to disk
 *
 *  Writes a volume sequence to disk. Will create one main file ([name].ivfs) and a series of ivf
 * volumes ([name]xx.ivf), one for each element in the sequence.
 *
 * @param volumes The volume sequence to export
 * @param name the name of the dataset, will be used for to name the output files [name].ivfs and
 * [name]xx.ivf
 * @param path path to the folder to put the main file
 * @param reltivePathToElements relative path (from the path to the main file) to where the sequence
 * elements will be written
 * @param overwrite whether or not to overwrite existing files.
 * @return string path to the created main-file
 */

IVW_MODULE_BASE_API std::string writeIvfVolumeSequence(const VolumeSequence& volumes,
                                                       std::string name, std::string path,
                                                       std::string reltivePathToElements = "",
                                                       bool overwrite = true);
}  // namespace util

}  // namespace inviwo

#endif  // IVW_IVFSEQUENCEVOLUMEWRITER_H
