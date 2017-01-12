#include <ZenLib/vdfs/fileIndex.h>
#include <iomanip>
#include "zTools.h"
#include "cli.h"
#include "Utils.h"

namespace Flags
{
    Cli::Flag unpackVdf("", "unpack-vdf", 2, " [vdf-archive, target-folder] "
            "Unpacks the given vdf-archive to the target-folder. "
            "Directory structure is not preserved.");
}

void unpackVdf()
{
    std::string vdf = Flags::unpackVdf.getArgs()[0];
    std::string target = Flags::unpackVdf.getArgs()[1];

    std::string vdfname = Utils::stripFilePath(Utils::stripExtension(vdf));
    target = target + "/" + vdfname;
    Utils::mkdir(target);

    VDFS::FileIndex idx;
    idx.loadVDF(vdf);

    // Print how many files we loaded
    std::cout << "Loaded " << idx.getKnownFiles().size() << " files into fileIndex!" << std::endl;

    for(auto& f : idx.getKnownFiles())
    {
        // Print some information about the file
        std::cout << "File: " << "(" << f.fileSize << " bytes) "<< target << "/" << f.fileName << " " << std::endl;

        std::vector<uint8_t> data;
        idx.getFileData(f, data);

        if(!Utils::writeFile(f.fileName, target, data))
            std::cout << " - Failed to write file!" << std::endl;
    }
}

bool ::zTools::tryRunTools()
{
    if(Flags::unpackVdf.isSet())
    {
        unpackVdf();
        return true;
    }

    return false;
}
