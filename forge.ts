import { createRequire } from "module";
import { fileURLToPath } from "url";
import path from "path";
import type { PassFlags } from "./PassFlags.generated";

export type { PassFlags };

const require = createRequire(import.meta.url);
const __dirname = path.dirname(fileURLToPath(import.meta.url));

interface IridiumForge {
  execute(
    version: string,
    path: string,
    code: Array<any>,
    buildContext: Array<any>,
    tick: (arg0: string) => void,
    tock: (arg0: string) => void,
    flags?: PassFlags,
  ): Array<any>;
}

const binaryWrapperPath = path.resolve(__dirname, "./forge.cjs");

const forge = require(binaryWrapperPath) as IridiumForge;

export const { execute } = forge;
export default forge;
