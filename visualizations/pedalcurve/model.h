
glm::vec2 circumcenter(glm::vec2 M, glm::vec2 A, glm::vec2 B) {

    const float EPSILON = 1E-12;

    float fabsy1y2 = abs(M.y - A.y);
    float fabsy2y3 = abs(A.y - B.y);

    float xc, yc, m1, m2, mx1, mx2, my1, my2, dx, dy;

    /* Check for coincident points */
    if(fabsy1y2 < EPSILON && fabsy2y3 < EPSILON) return M;

    if(fabsy1y2 < EPSILON) {
        m2  = -((B.x - A.x) / (B.y - A.y));
        mx2 = (A.x + B.x) / 2.0;
        my2 = (A.y + B.y) / 2.0;
        xc  = (A.x + M.x) / 2.0;
        yc  = m2 * (xc - mx2) + my2;
    }

    else if(fabsy2y3 < EPSILON) {
        m1  = -((A.x - M.x) / (A.y - M.y));
        mx1 = (M.x + A.x) / 2.0;
        my1 = (M.y + A.y) / 2.0;
        xc  = (B.x + A.x) / 2.0;
        yc  = m1 * (xc - mx1) + my1;
    }

    else {
        m1  = -((A.x - M.x) / (A.y - M.y));
        m2  = -((B.x - A.x) / (B.y - A.y));
        mx1 = (M.x + A.x) / 2.0;
        mx2 = (A.x + B.x) / 2.0;
        my1 = (M.y + A.y) / 2.0;
        my2 = (A.y + B.y) / 2.0;
        xc  = (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2);
        yc  = (fabsy1y2 > fabsy2y3) ?
        m1 * (xc - mx1) + my1 :
        m2 * (xc - mx2) + my2;
    }

    return glm::vec2(xc, yc);

}
