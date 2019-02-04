
namespace Mongoose
{
    UploadFile::UploadFile(std::string filename_, std::string data_)
        : filename(filename_),
        data(data_)
    {
    }

    std::string UploadFile::getName()
    {
        return filename;
    }

    std::string UploadFile::getData()
    {
        return data;
    }
    
    void UploadFile::saveTo(std::string directory)
    {
        //std::ostringstream oss;
        //oss << directory << "/" << filename;
        //std::fstream file;
        //file.open(oss.str().c_str(), std::fstream::out);
        //file << data;
        //file.close();
    }
}
