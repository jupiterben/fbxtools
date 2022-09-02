


class FBXUVChannel {
    constructor(private data: any) { }
    getPointUV(index: number) {
        const idx = this.data.indexArray[index];
        return this.data.directArray[idx];
    }
}

class FBXMesh {
    constructor(private data: any) { }
    get Name(): string { return this.data.name; }
    get ControlPoints(): Array<Array<number>> {
        return this.data.controlPoints;
    }
    get Polygons(): Array<Array<number>> {
        return this.data.polygons;
    }
    get UVChannels(): Array<FBXUVChannel> {
        return this.data.uv.map((element: any) => new FBXUVChannel(element));
    }

    getPoint(index: number) {
        return this.ControlPoints[index];
    }
    getPointCount() {
        return this.ControlPoints.length;
    }
    getPointUVs(chIndex: number, ptIndex: number) {
        const uvcList = this.UVChannels;
        const ch = uvcList[chIndex];
        const polygons = this.Polygons;
        let ret = Array<any>();
        let index = 0;
        for (let fi = 0; fi < polygons.length; ++fi) {
            for (let vi = 0; vi < polygons[fi].length; ++vi) {
                const pi = polygons[fi][vi];
                if (pi == ptIndex) {
                    ret.push(ch.getPointUV(index));
                }
                index++;
            }
        }
        return ret;
    }
}

class FBXNode {
    constructor(private data: any) {
    }

    getChild(index: number) {
        const d = this.data.children[index];
        return d && (new FBXNode(d));
    }
    getMesh() {
        return this.data.mesh && (new FBXMesh(this.data.mesh));
    }
}

class FBXContent {
    constructor(private data: any) {
    }
    getRoot() {
        return new FBXNode(this.data.RootNode);
    }
}


import * as fs from 'fs'

const rawData = fs.readFileSync(`${__dirname}/../test/mayaexport.json`, 'utf8');

const fbxContent = new FBXContent(JSON.parse(rawData!));
console.log(fbxContent);

const root = fbxContent.getRoot();
console.log(root);


const mesh = root.getChild(1).getMesh();
for (let i = 0; i < mesh.getPointCount(); i++) {
    console.log(mesh.getPointUVs(0, i));
}
