#include "zTools.h"
#include <iomanip>
#include "Utils.h"
#include "cli.h"
#include <ZenLib/vdfs/fileIndex.h>
#include <g-extract.h>
#include <ZenLib/utils/logger.h>

namespace Flags
{
    Cli::Flag unpackVdf("", "unpack-vdf", 2,
                                " [vdf-archive, target-folder] "
                                "Unpacks the given vdf-archive to the target-folder. "
                                "Directory structure is not preserved.");

    Cli::Flag installGame("", "install-game", 2,
                                " [installer-exe, target-folder] "
                                "Unpacks the given Gothic-Installer-Executable without actually running the installer.");
}

static void unpackVdf()
{
    std::string vdf = Flags::unpackVdf.getParam(0);
    std::string target = Flags::unpackVdf.getParam(1);

    std::string vdfname = Utils::stripFilePath(Utils::stripExtension(vdf));
    target = target + "/" + vdfname;
    Utils::mkdir(target);

    VDFS::FileIndex idx;
    idx.loadVDF(vdf);

    // Print how many files we loaded
    std::cout << "Loaded " << idx.getKnownFiles().size() << " files into fileIndex!" << std::endl;

    for (const auto& f : idx.getKnownFiles())
    {
        // Print some information about the file
        std::cout << "File: " << target << "/" << f << " " << std::endl;

        std::vector<uint8_t> data;
        idx.getFileData(f, data);

        if (!Utils::writeFile(f, target, data))
            std::cout << " - Failed to write file!" << std::endl;
    }
}

bool ::zTools::extractInstaller(const std::string& file, const std::string& targetLocation)
{
    try
    {
        LogInfo() << "Trying to extract Installer";
        GExtract::extractInstallerExecutable(file, targetLocation + "/installer-temp");

        std::string datacab = targetLocation + "/installer-temp/data1.cab";

        // FIXME: getCaseSensitivePath does not work on android?
        //std::string datacab = Utils::getCaseSensitivePath(datacabCaseInsensitive);

        if(datacab.empty())
        {
            LogError() << "Failed to find datacab at " << datacab;
            return false;
        }

        LogInfo() << "Extracting internal data-CAB-File";
        GExtract::extractInternalCABFile(datacab, targetLocation);
    }catch(std::runtime_error e)
    {
        LogError() << "Failed to extract installer: " << e.what();
        return false;
    }

    return true;
}

static void installGame()
{
    std::string installer = Flags::installGame.getParam(0);
    std::string target = Flags::installGame.getParam(1);

    if(!Utils::fileExists(installer))
    {
        LogError() << "Failed ot find file: " << installer;
        return;
    }

    zTools::extractInstaller(installer, target);
}


bool ::zTools::tryRunTools()
{
    if (Flags::unpackVdf.isSet())
    {
        unpackVdf();
        return true;
    }else if(Flags::installGame.isSet())
    {
        installGame();
        return true;
    }

    return false;
}
