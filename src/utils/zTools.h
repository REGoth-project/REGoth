//
// Created by desktop on 12.01.17.
//

#pragma once

/**
 * Commandline tools for specific tasks regarding Gothic
 */
namespace zTools
{
    /**
     * Called by main. Here's the point to check your flags and run your tools.
     * @return True, if a tool was ran. The program exits in that case.
     */
    bool tryRunTools();
}
